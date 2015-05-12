#include "pwdchangedialog.h"
#include "ui_pwdchangedialog.h"
#include <QMessageBox>
#include <QSqlQuery>

pwdChangeDialog::pwdChangeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::pwdChangeDialog)
{
    ui->setupUi(this);
    ui->oldPwdLineEdit->setFocus();
    setTabOrder(ui->oldPwdLineEdit,ui->newPwdLineEdit);
    setTabOrder(ui->newPwdLineEdit,ui->newPwdCLineEdit);
    ui->OkPwdBtn->setDefault(true);
}

pwdChangeDialog::~pwdChangeDialog()
{
    delete ui;
}

void pwdChangeDialog::on_OkPwdBtn_clicked()
{
    if(ui->oldPwdLineEdit->text().isEmpty())
        QMessageBox::information(this,tr("提示"),tr("登录密码为空！"),QMessageBox::Ok);

    QSqlQuery query;
    query.exec("select pwd from password");
    query.next();
    if(query.value(0).toString()!=ui->oldPwdLineEdit->text())
    {
        QMessageBox::information(this,tr("提示"),tr("密码错误！"),QMessageBox::Ok);
        ui->oldPwdLineEdit->clear();
        ui->newPwdLineEdit->clear();
        ui->newPwdCLineEdit->clear();
        ui->oldPwdLineEdit->setFocus();
    }
    else
    {
        if(ui->newPwdLineEdit->text().isEmpty() && ui->newPwdCLineEdit->text().isEmpty())
        {
            QMessageBox::information(this,tr("提示"),tr("新密码不能为空！"),QMessageBox::Ok);
        }
        else if(ui->newPwdLineEdit->text()!=ui->newPwdCLineEdit->text())
        {
            QMessageBox::information(this,tr("提示"),tr("密码不匹配！"),QMessageBox::Ok);
            ui->newPwdLineEdit->clear();
            ui->newPwdCLineEdit->clear();
            ui->newPwdLineEdit->setFocus();
        }
        else
        {
            QMessageBox confirm_Msg(QMessageBox::Warning,tr("密码修改"),tr("此过程不可逆，是否确认修改？"),
                                    QMessageBox::Yes | QMessageBox::No);
            if(confirm_Msg.exec()==QMessageBox::Yes)
            {
                QSqlDatabase::database().transaction();
                bool sucess1=query.exec("delete from password");
                bool sucess2=query.exec(QString("insert into password values('%1')").arg(ui->newPwdLineEdit->text()));
                if(sucess1 && sucess2)
                {
                    QSqlDatabase::database().commit();
                    QMessageBox::information(this,tr("提示"),tr("密码修改成功！"),QMessageBox::Ok);
                    this->close();
                }
                else
                    QSqlDatabase::database().rollback();
            }
        }

    }

}

void pwdChangeDialog::on_CancelPwdBtn_clicked()
{
    this->close();
}
