#include "SshHostConfigDialog.h"
#include "SshHostConfig.h"
#include "ui_SshHostConfigDialog.h"
#include <QDir>
#include <QFileDialog>
#include <QJsonObject>
#include <QMessageBox>

SshHostConfigDialog::SshHostConfigDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SshHostConfigDialog)
{
    ui->setupUi(this);

    // Connect the browse button to the slot
    connect(ui->buttonBrowseIdentityFile,
            &QPushButton::clicked,
            this,
            &SshHostConfigDialog::onBrowseIdentityFile);
}

SshHostConfigDialog::~SshHostConfigDialog()
{
    delete ui;
}

void SshHostConfigDialog::setSshHostConfig(const SshHostConfig &config)
{
    ui->lineEditHost->setText(config.host);
    ui->lineEditUser->setText(config.user);
    ui->lineEditIdentityFile->setText(config.identityFile);
}

SshHostConfig SshHostConfigDialog::sshHostConfig() const
{
    SshHostConfig config;
    config.host = ui->lineEditHost->text();
    config.user = ui->lineEditUser->text();
    config.identityFile = ui->lineEditIdentityFile->text();
    return config;
}

void SshHostConfigDialog::onBrowseIdentityFile()
{
    QString defaultDir = QDir::homePath() + "/.ssh";
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select Identity File"),
                                                    defaultDir,
                                                    tr("All Files (*)"));
    if (!fileName.isEmpty()) {
        ui->lineEditIdentityFile->setText(fileName);
    }
}
