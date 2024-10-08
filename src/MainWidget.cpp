#include "MainWidget.h"
#include "./ui_MainWidget.h"
#include "NewSessionDialog.h"
#include "ScreenSessionModel.h"
#include "ShellCommandExecutor.h"
#include "SshCommandExecutor.h"
#include "SshHostConfig.h"
#include "SshHostConfigDialog.h"
#include "SshSession.h"
#include <QDir>
#include <QFile>
#include <QFontMetrics>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include <QStandardPaths>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    loadHosts();

    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxyModel->setFilterKeyColumn(-1);
    ui->tableView->setModel(m_proxyModel);
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->editHostsButton, &QPushButton::clicked, this, &MainWidget::onEditHostsClicked);
    connect(ui->newSessionButton, &QPushButton::clicked, this, &MainWidget::onNewSessionClicked);
    connect(ui->refreshButton, &QPushButton::clicked, this, &MainWidget::refresh);
    connect(ui->hostComboBox,
            qOverload<int>(&QComboBox::currentIndexChanged),
            this,
            &MainWidget::onHostCurrentIndexChanged);
    connect(ui->viewScreenButton, &QPushButton::clicked, this, &MainWidget::onViewScreenClicked);
    connect(ui->tableView,
            &QTableView::customContextMenuRequested,
            this,
            &MainWidget::onCustomContextMenuRequested);
    connect(ui->filterLineEdit, &QLineEdit::textChanged, this, &MainWidget::onFilterChanged);

    int hostIndex = 0;
    if (!m_defaultHost.isEmpty()) {
        hostIndex = std::max(0, ui->hostComboBox->findText(m_defaultHost));
    }
    if (hostIndex == 0) {
        onHostCurrentIndexChanged(0);
    } else {
        ui->hostComboBox->setCurrentIndex(hostIndex);
    }
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::refresh()
{
    ui->refreshButton->setEnabled(false);
    qApp->processEvents();

    auto index = ui->hostComboBox->currentIndex();

    auto host = m_hosts[index];
    if (!host.isLocalhost() && !host.sshSession->isConnected()) {
        if (!host.sshSession->connectToHost()) {
            QMessageBox::critical(this, "Error", "Failed to connect to SSH host");
        }
    }

    if (host.isLocalhost() || host.sshSession->isConnected()) {
        host.screenModel->refresh();
    }

    ui->refreshButton->setEnabled(true);
}

void MainWidget::onHostCurrentIndexChanged(int index)
{
    if (index == -1) {
        m_proxyModel->setSourceModel(nullptr);
    } else {
        auto &host = m_hosts[index];
        if (m_proxyModel->sourceModel() != host.screenModel) {
            m_proxyModel->setSourceModel(host.screenModel);
            if (!m_wasHeaderSetup) {
                setupHeader();
            }
        }
        if (!host.wasRefreshed) {
            host.wasRefreshed = true;
            refresh();
        }
    }
}

void MainWidget::onViewScreenClicked()
{
    auto host = currentHost();
    auto session = selectedSession();
    if (host && session) {
        QString output = host->screenModel->screenManager()->retrieveSessionOutput(session->id);
        ui->plainTextEdit->setPlainText(output);
        ui->screenOfLabel->setText(tr("Screen of: ") + session->id + "@" + host->name);
    }
}

void MainWidget::onCustomContextMenuRequested()
{
    auto host = currentHost();
    if (!host) {
        return;
    }

    auto sessions = selectedSessions();
    if (!sessions.isEmpty()) {
        QMenu menu(this);
        menu.addAction("Quit", this, &MainWidget::onQuitScreenClicked);
        menu.exec(QCursor::pos());
    }
}

void MainWidget::onQuitScreenClicked()
{
    auto host = currentHost();
    if (!host) {
        return;
    }

    auto &screen = *host->screenModel->screenManager();
    auto sessions = selectedSessions();
    for (auto session : sessions) {
        screen.quit(session.id);
        if (!screen.hasSession(session.id)) {
            host->screenModel->remove(session.id);
        }
    }
}

void MainWidget::onNewSessionClicked()
{
    NewSessionDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        auto host = currentHost();
        if (host) {
            bool ok = host->screenModel->screenManager()
                          ->createSession(dialog.command(),
                                          dialog.workingDirectory(),
                                          dialog.sessionName(),
                                          dialog.isKeepSessionAfterCommandTerminatesChecked());
            if (ok) {
                host->screenModel->refresh();
            }
        }
    }
}

void MainWidget::onEditHostsClicked()
{
    SshHostConfigDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
    }

    QString sampleJson = R"(
{
    "default": "myserver.com",
    "hosts": [
        {
            "host": "localhost",
        },
        {
            "host": "myserver.com",
            "user": "john",
            "identityFile": "/home/john/.ssh/id_rsa"
        }
    ]
}
)";
    QMessageBox::information(this,
                             "Edit hosts",
                             QString("Editor not implemented yet.\nPlease edit the %1 file "
                                     "manually. The JSON format is:\n%2")
                                 .arg(hostsFilePath())
                                 .arg(sampleJson));
}

void MainWidget::onFilterChanged()
{
    m_proxyModel->setFilterWildcard(ui->filterLineEdit->text());
}

void MainWidget::addHost(const QString &name, const QString &user, const QString &identityFile)
{
    Host host;
    host.name = name;
    if (name == "localhost") {
        host.screenModel
            = new ScreenSessionModel(new ScreenManager(std::make_unique<ShellCommandExecutor>(),
                                                       this),
                                     this);
    } else {
        host.sshSession = new SshSession(this);
        host.sshSession->setHost(name);
        host.sshSession->setUser(user);
        host.sshSession->setIdentityFile(identityFile);

        host.screenModel
            = new ScreenSessionModel(new ScreenManager(std::make_unique<SshCommandExecutor>(
                                                           host.sshSession->session()),
                                                       this),
                                     this);
    }
    ui->hostComboBox->addItem(name);

    m_hosts.append(host);
}

const MainWidget::Host *MainWidget::currentHost() const
{
    return &m_hosts[ui->hostComboBox->currentIndex()];
}

const ScreenSession *MainWidget::selectedSession() const
{
    auto index = ui->tableView->currentIndex();
    if (!index.isValid()) {
        return nullptr;
    }
    index = m_proxyModel->mapToSource(index);
    return currentHost()->screenModel->session(index);
}

QList<ScreenSession> MainWidget::selectedSessions() const
{
    QList<ScreenSession> sessions;
    for (auto index : ui->tableView->selectionModel()->selectedIndexes()) {
        index = m_proxyModel->mapToSource(index);
        auto session = currentHost()->screenModel->session(index);
        if (session) {
            sessions.append(*session);
        }
    }
    return sessions;
}

QString MainWidget::hostsFilePath() const
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (!QDir(path).exists()) {
        QDir().mkpath(path);
    }
    path += "/hosts.json";
    return path;
}

void MainWidget::saveHosts()
{
    QJsonArray array;
    for (const auto &host : m_hosts) {
        SshHostConfig config;
        config.host = host.name;
        if (host.name != "localhost") {
            config.user = host.sshSession->user();
            config.identityFile = host.sshSession->identityFile();
        }
        array.append(config.toJson());
    }

    QJsonObject obj;
    obj.insert("hosts", array);
    QJsonDocument doc(obj);
    QString path = hostsFilePath();
    QFile file(path);
    if (file.open(QIODevice::WriteOnly)) {
        qDebug() << "Writing to file:" << file.fileName();
        file.write(doc.toJson());
    } else {
        qWarning() << "Failed to open file:" << path << "for writing:" << file.errorString();
    }
}

void MainWidget::loadHosts()
{
    ui->hostComboBox->clear();

    QString path = hostsFilePath();
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        qDebug() << "Reading from file:" << file.fileName();
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonObject obj = doc.object();
        QJsonArray array = obj.value("hosts").toArray();
        for (const auto &value : array) {
            auto config = SshHostConfig::fromJson(value.toObject());
            addHost(config.host, config.user, config.identityFile);
        }

        m_defaultHost = obj.value("default").toString().trimmed();
    }

    if (m_hosts.isEmpty()) {
        addHost("localhost", "", "");
    }
}

void MainWidget::setupHeader()
{
    m_wasHeaderSetup = true;
    QHeaderView *header = ui->tableView->horizontalHeader();
    header->setStretchLastSection(true);

    // Make sure that section 1 is wide enough for the string dateFormat
    header->resizeSection(ScreenSessionModel::StartedColumn,
                          header->fontMetrics().horizontalAdvance(ScreenSessionModel::dateFormat())
                              + 20);
}

bool MainWidget::Host::isLocalhost() const
{
    return name == "localhost";
}
