#include "NewSessionDialog.h"
#include "ui_NewSessionDialog.h"

NewSessionDialog::NewSessionDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NewSessionDialog)
{
    ui->setupUi(this);
}

NewSessionDialog::~NewSessionDialog()
{
    delete ui;
}

QString NewSessionDialog::command() const
{
    return ui->commandLineEdit->text().trimmed();
}

QString NewSessionDialog::workingDirectory() const
{
    return ui->workingDirectoryLineEdit->text().trimmed();
}

QString NewSessionDialog::sessionName() const
{
    return ui->sessionNameLineEdit->text().trimmed();
}
