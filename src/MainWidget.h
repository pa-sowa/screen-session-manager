#pragma once
#include <QWidget>

class ScreenSession;
class SshSession;
class ScreenSessionModel;
namespace TerminalSolution {
class TerminalView;
}

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWidget;
}
QT_END_NAMESPACE

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

private slots:
    void refresh();
    void onHostCurrentIndexChanged(int index);
    void onViewScreenClicked();
    void onCustomContextMenuRequested();
    void onQuitScreenClicked();
    void onNewSessionClicked();
    void onEditHostsClicked();

private:
    struct Host
    {
        QString name;
        SshSession *sshSession = nullptr;
        ScreenSessionModel *screenModel = nullptr;
        bool wasRefreshed = false;

        bool isLocalhost() const;
    };

    void addHost(const QString &name, const QString &user, const QString &identityFile);
    const Host *currentHost() const;
    const ScreenSession *selectedSession() const;
    QList<ScreenSession> selectedSessions() const;
    QString hostsFilePath() const;
    void saveHosts();
    void loadHosts();
    void setupHeader();
    void setupTerminal();

    Ui::MainWidget *ui;
    TerminalSolution::TerminalView *m_terminalView = nullptr;
    QList<Host> m_hosts;
    QString m_defaultHost;
    bool m_wasHeaderSetup = false;
};
