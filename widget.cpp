#include "widget.h"
#include "ui_widget.h"
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSplitter>
#include <QMessageBox>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "pieview.h"
#include <QTableView>
#include "pwdchangedialog.h"

Widget::Widget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    setFixedSize(750,500);
    createMenuBar();
    ui->stackedWidget->setCurrentIndex(0);

    QSqlQueryModel *factoryModel=new QSqlQueryModel(this);
    factoryModel->setQuery("select name from factory");
    ui->sellFactoryComboBox->setModel(factoryModel);

    QSplitter *splitter=new QSplitter(ui->managePage);
    splitter->resize(700,360);
    splitter->move(0,50);
    splitter->addWidget(ui->toolBox);
    splitter->addWidget(ui->dailyList);
    splitter->setStretchFactor(0,1);//?
    splitter->setStretchFactor(1,1);

    showDailyList();

    ui->factoryComboBox->setModel(factoryModel);
    createChartModelView();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::createMenuBar()
{
    manageAction=new QAction(tr("品牌车管理"),this);//创建操作
    chartAction=new QAction(tr("销售统计"),this);
    quitAction=new QAction(tr("退出"),this);

    manageAction->setShortcut(tr("Ctrl+M"));//设置快捷键
    chartAction->setShortcut(tr("Ctrl+C"));
    quitAction->setShortcut(tr("Ctrl+Q"));

    manageMenu=menuBar()->addMenu(tr("销售管理"));//menuBar()用于获取主窗口工具栏
    manageMenu->addAction(manageAction);
    manageMenu->addAction(chartAction);
    manageMenu->addSeparator();//添加分割符
    manageMenu->addAction(quitAction);

    passwordMenu=menuBar()->addMenu(tr("修改密码"));
    pwdChangeAction=new QAction(tr("修改密码"),this);
    pwdChangeAction->setShortcut(tr("Ctrl+A"));
    passwordMenu->addAction(pwdChangeAction);

    connect(manageAction,SIGNAL(triggered()),this,SLOT(on_manageMenu_clicked()));//信号关联
    connect(chartAction,SIGNAL(triggered()),this,SLOT(on_chartMenu_clicked()));
    connect(quitAction,SIGNAL(triggered()),this,SLOT(on_quitMenu_clicked()));
    connect(pwdChangeAction,SIGNAL(triggered()),this,SLOT(on_pwdChange_clicked()));

}

void Widget::on_manageMenu_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void Widget::on_chartMenu_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void Widget::on_quitMenu_clicked()
{
    QMessageBox message(QMessageBox::Warning,tr("退出"),tr("您是否确认退出？"),QMessageBox::Yes | QMessageBox::No);
    if(message.exec()==QMessageBox::Yes)
        this->close();//有没有this效果应该一样

}

void Widget::on_sellFactoryComboBox_currentIndexChanged(const QString &arg1)
{
    if(arg1=="请选择厂家")
    {
        on_sellCancelBtn_clicked();
    }
    else
    {
        ui->sellBrandComboBox->setEnabled(true);//有必要么？
        ui->sellNumSpinBox->setEnabled(true);
        QSqlQueryModel *model=new QSqlQueryModel(this);
        model->setQuery(QString("select name from brand where factory='%1'").arg(arg1));
        ui->sellBrandComboBox->setModel(model);
        ui->sellCancelBtn->setEnabled(true);
    }

}

void Widget::on_sellCancelBtn_clicked()
{
    ui->sellFactoryComboBox->setCurrentIndex(0);
    ui->sellBrandComboBox->clear();//?感觉没用
    ui->sellBrandComboBox->clearEditText();
    ui->sellBrandComboBox->setEnabled(false);
    ui->sellNumSpinBox->setEnabled(false);
}

void Widget::on_sellBrandComboBox_currentIndexChanged(const QString &arg1)
{
    ui->sellNumSpinBox->setValue(0);
    ui->sellNumSpinBox->setEnabled(false);
    ui->sellSumLineEdit->clear();
    ui->sellSumLineEdit->setEnabled(false);
    ui->sellOkBtn->setEnabled(false);

    QSqlQuery query;
    query.exec(QString("select price from brand where name='%1' and factory='%2'").arg(arg1).arg(
                   ui->sellFactoryComboBox->currentText()));
    query.next();
    ui->sellPriceLineEdit->setEnabled(false);
    ui->sellPriceLineEdit->setReadOnly(true);
    ui->sellPriceLineEdit->setText(query.value(0).toString());

    query.exec(QString("select last from brand where name='%1' and factory='%2'").arg(arg1).arg(
                   ui->sellFactoryComboBox->currentText()));
    query.next();
    int num=query.value(0).toInt();
    if(num==0)
    {
        QMessageBox::information(this,tr("提示"),tr("该品牌汽车已经售完！"),QMessageBox::Ok);
    }
    else
    {
        ui->sellNumSpinBox->setEnabled(true);
        ui->sellNumSpinBox->setMaximum(num);
        ui->sellLastNumLabel->setText(tr("剩余数量： %1").arg(num));
        ui->sellLastNumLabel->setVisible(true);
    }

}

void Widget::on_sellNumSpinBox_valueChanged(int arg1)
{
    if(arg1==0)
    {
        ui->sellSumLineEdit->clear();
        ui->sellSumLineEdit->setEnabled(false);
        ui->sellOkBtn->setEnabled(false);
    }
    else
    {
        ui->sellSumLineEdit->setEnabled(true);
        ui->sellSumLineEdit->setReadOnly(true);
        qreal sum=arg1*ui->sellPriceLineEdit->text().toInt();
        ui->sellSumLineEdit->setText(QString::number(sum));
        ui->sellOkBtn->setEnabled(true);
    }
}

void Widget::on_sellOkBtn_clicked()
{
    QString factory=ui->sellFactoryComboBox->currentText();
    QString name=ui->sellBrandComboBox->currentText();
    int value=ui->sellNumSpinBox->value();

    int last=ui->sellNumSpinBox->maximum()-value;

    QSqlQuery query;
    query.exec(QString("select sell from brand where name='%1' and factory='%2'").arg(name).arg(factory));
    query.next();
    int sell=query.value(0).toInt()+value;

    QSqlDatabase::database().transaction();
    bool rtn=query.exec(QString("update brand set sell = %1,last= %2 where name='%3' and factory='%4'").arg(sell).arg(
                            last).arg(name).arg(factory));

    QMessageBox buymessage(QMessageBox::Information,tr("购买信息"),
                           tr("您购买的车是：%1 %2，共%3辆，总计%4万元。").arg(ui->sellFactoryComboBox->currentText()).arg(
                            ui->sellBrandComboBox->currentText()).arg(ui->sellNumSpinBox->text()).arg(ui->sellSumLineEdit->text()),
                           QMessageBox::Yes | QMessageBox::No);
    if(buymessage.exec()==QMessageBox::Yes && rtn)
    {
        QSqlDatabase::database().commit();
        QMessageBox::information(this,tr("提示"),tr("购车成功"),QMessageBox::Ok);
        writeXml();
        showDailyList();
        on_sellCancelBtn_clicked();
    }
    else
    {
        QSqlDatabase::database().rollback();
    }
}

QString Widget::getDateTime(DateTimeType type)
{
    QDateTime datetime=QDateTime::currentDateTime();
    QString date=datetime.toString("yyyy-MM-dd");
    QString time=datetime.toString("hh:mm");
    QString dateAndTime=datetime.toString("yyyy-MM-dd  dddd hh:mm");
    if(type==Date)return date;
    else if(type==Time)return time;
    else return dateAndTime;
}

bool Widget::docRead()
{
    QFile file("data.xml");
    if(!file.open(QIODevice::ReadOnly))
        return false;
    QString Msg;
    int errorLine;
    int errorColumn;
    if(!doc.setContent(&file,false,&Msg,&errorLine,&errorColumn))
    {
        //注意，setContent（）函数极易出错，可以不使用默认参数进而判断出错点
        file.close();
        qDebug()<<Msg<<endl<<errorLine<<endl<<errorColumn<<endl;
        return false;
    }
    file.close();
    return true;
}

bool Widget::docWrite()
{
    QFile file("data.xml");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;
    QTextStream out(&file);
    doc.save(out,4);
    file.close();
    return true;
}

void Widget::writeXml()
{
    if(docRead())
    {
        QString currentDate=getDateTime(Date);
        QDomElement root=doc.documentElement();
        if(!root.hasChildNodes())
        {
            QDomElement date=doc.createElement(QString("日期"));
            QDomAttr curDate=doc.createAttribute("date");
            curDate.setValue(currentDate);
            date.setAttributeNode(curDate);
            root.appendChild(date);
            createNodes(date);
        }
        else
        {
            QDomElement date=root.lastChild().toElement();
            if(date.attribute("date")==currentDate)
            {
                createNodes(date);
            }
            else
            {
                QDomElement date=doc.createElement(QString("日期"));
                QDomAttr curDate=doc.createAttribute("date");
                curDate.setValue(currentDate);
                date.setAttributeNode(curDate);
                root.appendChild(date);
                createNodes(date);
            }
        }
        docWrite();
    }
}

void Widget::createNodes(QDomElement &date)
{
    QDomElement time=doc.createElement(QString("时间"));
    QDomAttr curTime=doc.createAttribute("time");
    curTime.setValue(getDateTime(Time));
    time.setAttributeNode(curTime);
    date.appendChild(time);
    QDomElement factory=doc.createElement(QString("厂家"));
    QDomElement brand=doc.createElement(QString("品牌"));
    QDomElement price=doc.createElement(QString("报价"));
    QDomElement num=doc.createElement(QString("数量"));
    QDomElement sum=doc.createElement(QString("金额"));

    QDomText text;
    text=doc.createTextNode(QString("%1").arg(ui->sellFactoryComboBox->currentText()));
    factory.appendChild(text);
    text=doc.createTextNode(QString("%1").arg(ui->sellBrandComboBox->currentText()));
    brand.appendChild(text);
    text=doc.createTextNode(QString("%1").arg(ui->sellPriceLineEdit->text()));
    price.appendChild(text);
    text=doc.createTextNode(QString("%1").arg(ui->sellNumSpinBox->value()));
    num.appendChild(text);
    text=doc.createTextNode(QString("%1").arg(ui->sellSumLineEdit->text()));
    sum.appendChild(text);

    time.appendChild(factory);
    time.appendChild(brand);
    time.appendChild(price);
    time.appendChild(num);
    time.appendChild(sum);
}

void Widget::showDailyList()
{
    ui->dailyList->clear();
    if(docRead())
    {
        QDomElement root=doc.documentElement();
        QString title=root.tagName();
        QListWidgetItem *titleItem=new QListWidgetItem;
        titleItem->setText(QString("------%1------").arg(title));
        titleItem->setTextAlignment(Qt::AlignCenter);
        ui->dailyList->addItem(titleItem);

        if(root.hasChildNodes())
        {
            QString currentDate=getDateTime(Date);
            QDomElement dateElement=root.lastChild().toElement();
            QString date=dateElement.attribute("date");

            if(date==currentDate)
            {
                ui->dailyList->addItem("");
                ui->dailyList->addItem(QString("日期:%1").arg(date));
                ui->dailyList->addItem("");
                QDomNodeList children=dateElement.childNodes();
                for(int i=0;i<children.count();++i)
                {
                    QDomNode node=children.at(i);
                    QString time=node.toElement().attribute("time");
                    QDomNodeList list=node.childNodes();

                    QString factory=list.at(0).toElement().text();
                    QString brand=list.at(1).toElement().text();
                    QString price=list.at(2).toElement().text();
                    QString num=list.at(3).toElement().text();
                    QString sum=list.at(4).toElement().text();

                    QString str=time+"出售"+brand+factory+"  "+num+"辆, "+"成交价："+price+"万，共"+sum+"万元";

                    QListWidgetItem *tempItem=new QListWidgetItem;
                    tempItem->setText("****************************");
                    tempItem->setTextAlignment(Qt::AlignCenter);
                    ui->dailyList->addItem(tempItem);
                    ui->dailyList->addItem(str);
                }
            }
        }
    }
}

void Widget::createChartModelView()
{
    chartModel=new QStandardItemModel(this);
    chartModel->setColumnCount(2);
    chartModel->setHeaderData(0,Qt::Horizontal,QString("品牌"));
    chartModel->setHeaderData(1,Qt::Horizontal,QString("销售数量"));
    QSplitter *splitter=new QSplitter(ui->chartPage);
    splitter->resize(700,320);
    splitter->move(0,80);

    QTableView *table=new QTableView;
    PieView *pieChart=new PieView;
    splitter->addWidget(table);
    splitter->addWidget(pieChart);
    splitter->setStretchFactor(0,1);
    splitter->setStretchFactor(1,2);
    table->setModel(chartModel);
    pieChart->setModel(chartModel);

    QItemSelectionModel *selectionModel=new QItemSelectionModel(chartModel);
    table->setSelectionModel(selectionModel);
    pieChart->setSelectionModel(selectionModel);
}

void Widget::showChart()
{
    QSqlQuery query;
    query.exec(QString("select name,sell from brand where factory='%1'").arg(ui->factoryComboBox->currentText()));
    chartModel->removeRows(0,chartModel->rowCount(QModelIndex()),QModelIndex());
    int row=0;

    while(query.next())
    {
        int r=qrand()%256;
        int g=qrand()%256;
        int b=qrand()%256;

        chartModel->insertRows(row,1,QModelIndex());
        chartModel->setData(chartModel->index(row,0,QModelIndex()),query.value(0).toString());
        chartModel->setData(chartModel->index(row,1,QModelIndex()),query.value(1).toInt());
        chartModel->setData(chartModel->index(row,0,QModelIndex()),QColor(r,g,b),Qt::DecorationRole);
        row++;
    }
}

void Widget::on_factoryComboBox_currentIndexChanged(const QString &arg1)
{
    if(arg1!="请选择厂家")
        showChart();
}

void Widget::on_updateBtn_clicked()
{
    if(ui->factoryComboBox->currentText()!="请选择厂家")
        showChart();
}

void Widget::on_pwdChange_clicked()
{
    pwdChangeDialog *pwdDialog=new pwdChangeDialog(this);
    pwdDialog->show();
}
