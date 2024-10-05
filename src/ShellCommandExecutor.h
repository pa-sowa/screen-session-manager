#pragma once
#include "AbstractCommandExecutor.h"

class ShellCommandExecutor : public AbstractCommandExecutor
{
public:
    std::optional<QByteArray> executeCommand(const QString &command) override;
};
