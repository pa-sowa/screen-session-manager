#pragma once
#include <QDialog>

namespace Ui {
class NewSessionDialog;
}

class NewSessionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewSessionDialog(QWidget *parent = nullptr);
    ~NewSessionDialog();

    QString command() const;
    QString workingDirectory() const;
    QString sessionName() const;

private:
    Ui::NewSessionDialog *ui;
};
