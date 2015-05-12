#include "logindialog.h"
#include "ui_logindialog.h"
#include <QMessageBox>
#include <QSqlQuery>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    setFixedSize(400,300);
    setWindowTitle(tr("登录"));
    ui->pwdLineEdit->setFocus();//这条语句和下面这条语句意思是不一样的
    ui->loginBtn->setDefault(true);
    int i=0;
    while(i<100000000)//纯属延时
    {
        i++;
    }
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_loginBtn_clicked()
{
    if(ui->pwdLineEdit->text().isEmpty())
    {
        QMessageBox::information(this,tr("请输入密码"),tr("请先输入密码再登录!"),QMessageBox::Ok);
        ui->pwdLineEdit->setFocus();
    }
    else
    {
        QSqlQuery query;
        query.exec("select pwd from password");
        query.next();

        if(query.value(0).toString()==ui->pwdLineEdit->text())
        {
            QDialog::accept();
        }
        else
        {
            QMessageBox::warning(this,tr("密码错误"),tr("请输入正确密码再登录！"),QMessageBox::Ok);
            ui->pwdLineEdit->clear();
            ui->pwdLineEdit->setFocus();
        }
    }
}

void LoginDialog::on_quitBtn_clicked()
{
    QDialog::reject();
}
