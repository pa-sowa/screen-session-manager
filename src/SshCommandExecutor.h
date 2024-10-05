#pragma once
#include "AbstractCommandExecutor.h"

struct ssh_session_struct;

class SshCommandExecutor : public AbstractCommandExecutor
{
public:
    SshCommandExecutor(ssh_session_struct *session);

    std::optional<QByteArray> executeCommand(const QString &command) override;

private:
    ssh_session_struct *m_session;
};
