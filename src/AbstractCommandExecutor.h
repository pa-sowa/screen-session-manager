#pragma once
#include <optional>
#include <QStringList>

class AbstractCommandExecutor
{
public:
    virtual ~AbstractCommandExecutor();

    virtual std::optional<QByteArray> executeCommand(const QString &command) = 0;

    std::optional<QStringList> executeCommandLines(const QString &command);
};
