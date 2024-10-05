#include "SshCommandExecutor.h"
#include <libssh/libssh.h>
#include <QDebug>
#include <QString>

namespace {

// Custom deleter for ssh_channel
struct SshChannelDeleter
{
    void operator()(ssh_channel channel) const
    {
        if (channel != nullptr) {
            if (ssh_channel_is_open(channel)) {
                ssh_channel_close(channel);
            }
            ssh_channel_free(channel);
        }
    }
};

using SshChannelPtr = std::unique_ptr<ssh_channel_struct, SshChannelDeleter>;

} // anonymous namespace

SshCommandExecutor::SshCommandExecutor(ssh_session_struct *session)
    : m_session(session)
{}

std::optional<QByteArray> SshCommandExecutor::executeCommand(const QString &command)
{
    SshChannelPtr channel{ssh_channel_new(m_session)};
    if (!channel) {
        qWarning() << "Failed to create SSH channel";
        return {};
    }

    if (ssh_channel_open_session(channel.get()) != SSH_OK) {
        qWarning() << "Failed to open SSH channel";
        return {};
    }

    if (ssh_channel_request_exec(channel.get(), command.toUtf8()) != SSH_OK) {
        qWarning() << "Failed to execute SSH command: " << command;
        return {};
    }

    char buffer[256];
    int nbytes;
    QString bufferString; // Used to store and process partial lines
    QByteArray output;

    while ((nbytes = ssh_channel_read(channel.get(), buffer, sizeof(buffer), 0)) > 0) {
        output.append(buffer, nbytes);
    }

    return output;
}
