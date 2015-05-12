#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMainWindow>
#include <QMenuBar>
#include "qdom.h"
#include <QDomDocument>
#include <QStandardItemModel>

namespace Ui {
class Widget;
}

class Widget : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    QMenu *manageMenu;
    QMenu *passwordMenu;

    QAction *manageAction;
    QAction *chartAction;
    QAction *quitAction;
    QAction *pwdChangeAction;
    enum DateTimeType{Time,Date,DateTime};
    QString getDateTime(DateTimeType type);

private slots:
    void on_manageMenu_clicked();
    void on_chartMenu_clicked();
    void on_quitMenu_clicked();

    
    void on_sellFactoryComboBox_currentIndexChanged(const QString &arg1);

    void on_sellCancelBtn_clicked();

    void on_sellBrandComboBox_currentIndexChanged(const QString &arg1);

    void on_sellNumSpinBox_valueChanged(int arg1);

    void on_sellOkBtn_clicked();

    void on_factoryComboBox_currentIndexChanged(const QString &arg1);

    void on_updateBtn_clicked();

    void on_pwdChange_clicked();

private:
    Ui::Widget *ui;
    void createMenuBar();

    QDomDocument doc;
    bool docRead();
    bool docWrite();
    void writeXml();
    void createNodes(QDomElement &date);
    void showDailyList();
    QStandardItemModel *chartModel;

    void createChartModelView();
    void showChart();
};

#endif // WIDGET_H
