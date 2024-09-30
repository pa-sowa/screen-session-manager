#include "ShellCommandExecutor.h"
#include <QDebug>
#include <QProcess>

std::optional<QStringList> ShellCommandExecutor::executeCommand(const QString &command)
{
    QProcess process;
    process.startCommand(command);
    process.waitForFinished();

    if (process.exitCode() != 0) {
        qWarning() << "Command \"" << command << "\" failed with exit code" << process.exitCode();
        return std::nullopt;
    }

    // Split the output into lines and return
    QString output = process.readAllStandardOutput();
    QStringList outputLines = output.split('\n', Qt::SkipEmptyParts);
    return outputLines;
}
