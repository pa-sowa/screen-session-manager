#include "SshCommandExecutor.h"
#include <libssh/libssh.h>
#include <QDebug>
#include <QString>

SshCommandExecutor::SshCommandExecutor(ssh_session_struct *session)
    : m_session(session)
{}

std::optional<QStringList> SshCommandExecutor::executeCommand(const QString &command)
{
    ssh_channel channel = ssh_channel_new(m_session);
    if (channel == nullptr) {
        qWarning() << "Failed to create SSH channel";
        return std::nullopt;
    }

    if (ssh_channel_open_session(channel) != SSH_OK) {
        qWarning() << "Failed to open SSH channel";
        ssh_channel_free(channel);
        return std::nullopt;
    }

    if (ssh_channel_request_exec(channel, command.toUtf8()) != SSH_OK) {
        qWarning() << "Failed to execute SSH command: " << command;
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return std::nullopt;
    }

    char buffer[256];
    int nbytes;
    QString bufferString; // Used to store and process partial lines
    QStringList output;

    while ((nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0)) > 0) {
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

    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return output;
}
