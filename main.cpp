#include "widget.h"
#include <QApplication>
#include "connection.h"
#include <QSqlDatabase>
#include <QDebug>
#include <QStringList>
#include <QString>
#include "logindialog.h"
#include <QPixmap>
#include <QSplashScreen>
#include <QImage>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    /*QSqlDatabase db=QSqlDatabase::addDatabase("QMYSQL");
    qDebug()<<db.open()<<endl;

    QStringList drivers=QSqlDatabase::drivers();
    foreach (QString driver,drivers) {
        qDebug()<<driver<<endl;

    }*/

    //QImage image("9387426_201412590187_2.png");
    QPixmap pixmap(400,300);
    pixmap.load("u=3066504418,2270399769&fm=23&gp=0.png");


    QSplashScreen splash(pixmap);
    splash.show();
    if(!createConnection() || !createXml())return 0;

    Widget w;
    LoginDialog dlg;
    dlg.show();
    splash.finish(&dlg);
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
