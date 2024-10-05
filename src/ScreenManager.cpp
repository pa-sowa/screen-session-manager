#include "ScreenManager.h"
#include <QRegularExpression>

ScreenManager::ScreenManager(std::unique_ptr<AbstractCommandExecutor> executor, QObject *parent)
    : QObject(parent)
    , m_executor(std::move(executor))
{}

QList<ScreenSession> ScreenManager::listSessions() const
{
    QList<ScreenSession> sessions;

    std::optional<QStringList> output = m_executor->executeCommandLines(
        "bash -c \"LANG=en_US.UTF-8 LC_ALL=en_US.UTF-8 screen -ls\"");
    if (output) {
        QRegularExpression regex(
            R"(\s*(\d+\.\S+)\s+\((\d{2}/\d{2}/\d{4} \d{2}:\d{2}:\d{2} (AM|PM))\)\s+\((Attached|Detached)\))");
        for (const QString &line : *output) {
            QRegularExpressionMatch match = regex.match(line);
            if (match.hasMatch()) {
                ScreenSession sessionData;
                sessionData.id = match.captured(1);
                sessionData.attached = (match.captured(3) == "Attached");
                sessionData.started = QDateTime::fromString(match.captured(2),
                                                            "MM/dd/yyyy hh:mm:ss AP");
                sessions.append(sessionData);
            }
        }
    }

    return sessions;
}

QByteArray ScreenManager::retrieveSessionOutput(const QString &sessionId) const
{
    QString filePath = "/tmp/" + sessionId + ".hardcopy";
    QString command = "screen -S " + sessionId + " -X hardcopy " + filePath;
    std::optional<QByteArray> output = m_executor->executeCommand(command);
    if (output) {
        command = "cat " + filePath;
        std::optional<QByteArray> sessionOutput = m_executor->executeCommand(command);
        m_executor->executeCommand("rm " + filePath);
        if (sessionOutput) {
            return *sessionOutput;
        }
    }

    return {};
}

std::optional<ScreenManager::Process> ScreenManager::lastProcess(const QString &sessionId) const
{
    auto processes = listProcesses(sessionId);
    if (processes.size() > 1) {
        return processes.last();
    }
    return {};
}

QList<ScreenManager::Process> ScreenManager::listProcesses(const QString &sessionId) const
{
    ScreenSession session;
    session.id = sessionId;

    QString command = QString("pstree -p %1").arg(session.pid());
    std::optional<QStringList> output = m_executor->executeCommandLines(command);
    if (output) {
        QList<Process> ret;
        QStringList processes = (*output)[0].split("---");
        for (const QString &process : processes) {
            Process p;
            p.pid = process.split('(')[1].split(')')[0].toUInt();
            p.name = process.split('(')[0].trimmed();
            ret.append(p);
        }
        return ret;
    }

    return {};
}

bool ScreenManager::isIdle(const QString &sessionId) const
{
    return lastProcess(sessionId)->name == "bash";
}

QString ScreenManager::workingDirectory(const QString &sessionId) const
{
    return workingDirectory(listProcesses(sessionId).last().pid);
}

QString ScreenManager::workingDirectory(quint32 processPid) const
{
    std::optional<QStringList> output = m_executor->executeCommandLines(
        QString("readlink /proc/%1/cwd").arg(processPid));
    if (output && !output->isEmpty()) {
        return (*output)[0];
    }
    return {};
}

void ScreenManager::quit(const QString &sessionId)
{
    m_executor->executeCommand("screen -S " + sessionId + " -X quit");
}

bool ScreenManager::hasSession(const QString &sessionId) const
{
    return listSessions().contains(ScreenSession{false, sessionId, QDateTime()});
}

bool ScreenManager::createSession(const QString command,
                                  const QString &cwd,
                                  const QString &sessionId,
                                  bool keepSession)
{
    QString cmd = "screen -dm";
    if (!sessionId.isEmpty()) {
        cmd += "S " + sessionId;
    }

    cmd += " bash -c \"";
    if (!cwd.isEmpty()) {
        cmd += "cd '" + cwd + "' && ";
    }

    QStringList commandArgs = command.split(' ', Qt::SkipEmptyParts);
    for (const QString &arg : commandArgs) {
        cmd += "'" + arg + "' ";
    }
    if (keepSession) {
        cmd += "&& exec bash";
    }
    cmd += "\"";

    auto ret = m_executor->executeCommand(cmd);
    if (ret) {
        return true;
    } else {
        return false;
    }
}

quint32 ScreenSession::pid() const
{
    return id.split('.')[0].toUInt();
}

bool ScreenSession::operator==(const ScreenSession &other) const
{
    return id == other.id;
}

QString ScreenManager::Process::toString() const
{
    return name;
}
