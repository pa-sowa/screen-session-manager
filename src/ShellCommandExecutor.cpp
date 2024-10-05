#include "ShellCommandExecutor.h"
#include <QDebug>
#include <QProcess>

std::optional<QByteArray> ShellCommandExecutor::executeCommand(const QString &command)
{
    QProcess process;
    process.startCommand(command);
    process.waitForFinished();

    if (process.exitCode() != 0) {
        qWarning() << "Command \"" << command << "\" failed with exit code" << process.exitCode();
        return std::nullopt;
    }

    return process.readAllStandardOutput();
}
