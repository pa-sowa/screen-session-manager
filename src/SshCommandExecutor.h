#pragma once
#include "ICommandExecutor.h"

struct ssh_session_struct;

class SshCommandExecutor : public ICommandExecutor
{
public:
    SshCommandExecutor(ssh_session_struct *session);

    std::optional<QStringList> executeCommand(const QString &command) override;

private:
    ssh_session_struct *m_session;
};
