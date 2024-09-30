#include "SshSession.h"
#include <libssh/libssh.h>
#include <QDebug>

SshSession::SshSession(QObject *parent)
    : QObject{parent}
{
    m_session = ssh_new();
}

SshSession::~SshSession()
{
    if (isConnected()) {
        ssh_disconnect(m_session);
    }
    ssh_free(m_session);
}

void SshSession::setHost(const QString &host)
{
    ssh_options_set(m_session, SSH_OPTIONS_HOST, host.toUtf8());
}

QString SshSession::host() const
{
    return option(SSH_OPTIONS_HOST);
}

void SshSession::setUser(const QString &user)
{
    ssh_options_set(m_session, SSH_OPTIONS_USER, user.toUtf8());
}

QString SshSession::user() const
{
    return option(SSH_OPTIONS_USER);
}

void SshSession::setIdentityFile(const QString &identityFile)
{
    ssh_options_set(m_session, SSH_OPTIONS_IDENTITY, identityFile.toUtf8());
}

QString SshSession::identityFile() const
{
    return option(SSH_OPTIONS_IDENTITY);
}

bool SshSession::connectToHost()
{
    if (ssh_connect(m_session) != SSH_OK) {
        qWarning() << "Failed to connect to SSH host " << host();
        return false;
    }

    if (ssh_userauth_publickey_auto(m_session, nullptr, nullptr) != SSH_AUTH_SUCCESS) {
        qWarning() << "Failed to authenticate to SSH host " << host();
        ssh_disconnect(m_session);
        return false;
    }

    return true;
}

bool SshSession::isConnected() const
{
    return ssh_is_connected(m_session);
}

QString SshSession::option(int sshOption) const
{
    QString ret;
    char *value = nullptr;
    if (ssh_options_get(m_session, static_cast<ssh_options_e>(sshOption), &value) == SSH_OK) {
        ret = QString::fromUtf8(value);
        ssh_string_free_char(value);
    }
    return ret;
}

void SshSession::disconnectFromHost()
{
    ssh_disconnect(m_session);
}
