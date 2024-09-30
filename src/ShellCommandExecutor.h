#pragma once
#include "ICommandExecutor.h"

class ShellCommandExecutor : public ICommandExecutor
{
public:
    std::optional<QStringList> executeCommand(const QString &command) override;
};
