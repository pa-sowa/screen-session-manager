#pragma once
#include "ICommandExecutor.h"
#include <memory>
#include <optional>
#include <QDateTime>
#include <QObject>

struct ScreenSession
{
    bool attached;
    QString id;
    QDateTime started;

    quint32 pid() const;

    bool operator==(const ScreenSession &other) const;
};

class ScreenManager : public QObject
{
    Q_OBJECT
public:
    struct Process
    {
        quint32 pid = 0;
        QString name;

        QString toString() const;
    };

    ScreenManager(std::unique_ptr<ICommandExecutor> executor, QObject *parent = nullptr);

    QList<ScreenSession> listSessions() const;
    QString retrieveSessionOutput(const QString &sessionId) const;
    /// Returns the last process in the session (not including screen).
    std::optional<Process> lastProcess(const QString &sessionId) const;
    QList<Process> listProcesses(const QString &sessionId) const;
    /// Returns true if the session is idle (bash is the only process running).
    bool isIdle(const QString &sessionId) const;
    QString workingDirectory(const QString &sessionId) const;
    QString workingDirectory(quint32 processPid) const;

    void quit(const QString &sessionId);
    bool hasSession(const QString &sessionId) const;

    bool createSession(const QString command,
                       const QString &cwd = {},
                       const QString &sessionId = {});

private:
    std::unique_ptr<ICommandExecutor> m_executor;
};
