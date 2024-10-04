#pragma once
#include <QDialog>
#include <QString>

struct SshHostConfig;

namespace Ui {
class SshHostConfigDialog;
}

class SshHostConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SshHostConfigDialog(QWidget *parent = nullptr);
    ~SshHostConfigDialog();

    // Set and get SshHostConfig structure
    void setSshHostConfig(const SshHostConfig &config);
    SshHostConfig sshHostConfig() const;

private slots:
    // Slot for browsing the identity file
    void onBrowseIdentityFile();

private:
    Ui::SshHostConfigDialog *ui;
};
