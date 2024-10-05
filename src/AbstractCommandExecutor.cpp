#include "AbstractCommandExecutor.h"

AbstractCommandExecutor::~AbstractCommandExecutor() = default;

std::optional<QStringList> AbstractCommandExecutor::executeCommandLines(const QString &command)
{
    if (auto output = executeCommand(command)) {
        return QString::fromUtf8(*output).split('\n', Qt::SkipEmptyParts);
    } else {
        return {};
    }
}
