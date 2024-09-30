#pragma once
#include <optional>
#include <QStringList>

class ICommandExecutor
{
public:
    virtual ~ICommandExecutor() = default;

    virtual std::optional<QStringList> executeCommand(const QString &command) = 0;
};
