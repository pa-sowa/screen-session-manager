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

std::optional<QStringList> SshCommandExecutor::executeCommand(const QString &command)
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
    QStringList output;

    while ((nbytes = ssh_channel_read(channel.get(), buffer, sizeof(buffer), 0)) > 0) {
        // Append data to bufferString and split into lines
        bufferString += QString::fromUtf8(buffer, nbytes);

        int newLineIndex;
        while ((newLineIndex = bufferString.indexOf('\n')) != -1) {
            QString line = bufferString.left(newLineIndex).trimmed();
            output.append(line); // Add the full line to output

            // Remove the processed line from bufferString
            bufferString = bufferString.mid(newLineIndex + 1);
        }
    }

    // Add any remaining data in bufferString as the last line if it's non-empty
    if (!bufferString.isEmpty()) {
        output.append(bufferString.trimmed());
    }
    return output;
}
