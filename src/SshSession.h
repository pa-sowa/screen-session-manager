#pragma once
#include <QObject>

struct ssh_session_struct;

class SshSession : public QObject
{
    Q_OBJECT
public:
    explicit SshSession(QObject *parent = nullptr);
    ~SshSession();

    void setHost(const QString &host);
    QString host() const;
    void setUser(const QString &user);
    QString user() const;
    void setIdentityFile(const QString &identityFile);
    QString identityFile() const;

    bool connectToHost();
    void disconnectFromHost();
    bool isConnected() const;

    ssh_session_struct *session() const { return m_session; }

signals:

private:
    QString option(int sshOption) const;

    ssh_session_struct *m_session = nullptr;
};
