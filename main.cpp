#include "widget.h"
#include <QApplication>
#include "connection.h"
#include <QSqlDatabase>
#include <QDebug>
#include <QStringList>
#include <QString>
#include "logindialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    /*QSqlDatabase db=QSqlDatabase::addDatabase("QMYSQL");
    qDebug()<<db.open()<<endl;

    QStringList drivers=QSqlDatabase::drivers();
    foreach (QString driver,drivers) {
        qDebug()<<driver<<endl;

    }*/

    if(!createConnection() || !createXml())return 0;

    Widget w;
    LoginDialog dlg;
    if(dlg.exec()==QDialog::Accepted)
    {
        w.show();
        return a.exec();
    }
    else
    {
        return 0;
    }
}
