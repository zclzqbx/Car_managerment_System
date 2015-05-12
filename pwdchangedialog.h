#ifndef PWDCHANGEDIALOG_H
#define PWDCHANGEDIALOG_H

#include <QDialog>

namespace Ui {
class pwdChangeDialog;
}

class pwdChangeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit pwdChangeDialog(QWidget *parent = 0);
    ~pwdChangeDialog();

private slots:
    void on_OkPwdBtn_clicked();

    void on_CancelPwdBtn_clicked();

private:
    Ui::pwdChangeDialog *ui;
};

#endif // PWDCHANGEDIALOG_H
