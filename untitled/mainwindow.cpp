#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "loginwin.h"
#include "settingwin.h"
#include "flatui.h"
#include <QTime>
#include <QList>
#include <QIcon>
#include <QMessageBox>
#include <synchapi.h>
#include <QVector>
#include <QTextCursor>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("智能设备管理系统");
    this->setWindowIcon(QIcon(":/new/prefix1/image/main_person.png"));

    temperature = 27;
    humidity = 64;
    smoke = 35;
    air = 17;
    Illuminance = 46;
    fire = 29;

    temperature_tmep = 0;
    humidity_tmep = 0;
    smoke_tmep = 0;
    air_tmep = 0;
    Illuminance_tmep = 0;
    fire_tmep = 0;

    warnniing = 1;

    FlatUI::Instance()->setPushButtonQss(ui->pushButton);
    FlatUI::Instance()->setPushButtonQss(ui->pushButton_2, 5, 8, "#E74C3C", "#FFFFFF", "#EC7064", "#FFF5E7", "#DC2D1A", "#F5A996");
    FlatUI::Instance()->setPushButtonQss(ui->pushButton_3);
    FlatUI::Instance()->setPushButtonQss(ui->pushButton_4, 5, 8, "#E74C3C", "#FFFFFF", "#EC7064", "#FFF5E7", "#DC2D1A", "#F5A996");

    serial = new QSerialPort("COM4");
    serial->open(QIODevice::ReadWrite);     //打开读写
    serial->setBaudRate(QSerialPort::Baud115200);  //波特率
    serial->setDataBits(QSerialPort::Data8); //8位数据位
    serial->setParity(QSerialPort::NoParity);    //无奇偶校验
    serial->setStopBits(QSerialPort::OneStop);   //1位停止位
    serial->setFlowControl(QSerialPort::NoFlowControl);  //无控制

    connect(serial,SIGNAL(readyRead()),this,SLOT(serialRead()));    //连接槽

    connect(this, SIGNAL(signal_temperature()), this, SLOT(slots_temperature()));
    connect(this, SIGNAL(signal_humidity()), this, SLOT(slots_humidity()));
    connect(this, SIGNAL(signal_smoke()), this, SLOT(slots_smoke()));
    connect(this, SIGNAL(signal_air()), this, SLOT(slots_air()));
    connect(this, SIGNAL(signal_Illuminance()), this, SLOT(slots_Illuminance()));
    connect(this, SIGNAL(signal_fire()), this, SLOT(slots_fire()));
    connect(this, SIGNAL(signal_Person()), this, SLOT(slots_Person()));
    connect(this, SIGNAL(signal_Rain()), this, SLOT(slots_Rain()));

    if(QSqlDatabase::contains("qt_sql_default_connection"))
    {
        database = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        database = QSqlDatabase::addDatabase("QSQLITE");
    }

    database.setDatabaseName(QString(".\\device.db"));
    //database.setDatabaseName(QString("C:\\Users\\hasee-pc\\Desktop\\test\\device\\device.db"));
    if (!database.open())
    {
        qDebug() << "Error: Failed to connect database." << database.lastError();
    }
    else
    {
        qDebug() << "Succeed to connect database." ;
    }


    if(database.tables().contains("sensor"))
    {
        qDebug() << "sensor 表存在";
    }
    else
    {
        qDebug() << "sensor 表不存在！！";
    }

    Sleep(10);

    QSqlQuery query;//新建查询实例
    QString select_sql = "select * from setting";
    if(!query.exec(select_sql))
    {
        qDebug()<<query.lastError();
    }
    else
    {
        while(query.next())
        {
            temperature = query.value(0).toInt();
            humidity = query.value(1).toInt();
            smoke = query.value(2).toInt();
            air = query.value(3).toInt();
            Illuminance = query.value(4).toInt();
            fire = query.value(5).toInt();

            qDebug()<<temperature<<humidity<<smoke<<air<<Illuminance<<fire;
//            qDebug()<<Sql_Temperature<<Sql_Humidity<<Sql_Smoke<<Sql_Air<<Sql_Illuminance<<Sql_Fire;
        }
    }

    /*
    //数据传送至textEdit控件
    QByteArray qa = serial->readAll();

    ui->textEdit->append(qa);

    ui->label->setText("读取中...");

    serialRead(qa);
    */
}

MainWindow::~MainWindow()
{
    database.close();
    delete ui;
}



void MainWindow::serialRead()
{
    static QByteArray qa;
    //QVector<QString>qa;
    qa += serial->readAll();
    if(qa.endsWith("R"))
    {
        QTextCursor cursor=ui->textEdit->textCursor();
        cursor.movePosition(QTextCursor::End);
        ui->textEdit->setTextCursor(cursor);
        ui->textEdit->insertPlainText(qa);
        ui->textEdit->insertPlainText(QString("\n\n"));

#if 0
    //数据传送至textEdit控件
    QByteArray qa = serial->readAll();
#endif
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");

    ui->lineEdit->setText(current_date);
    ui->textEdit->insertPlainText(current_date);
    ui->textEdit->insertPlainText(QString("   "));
    ui->textEdit->insertPlainText(qa);
    ui->textEdit->insertPlainText(QString("\n"));
//    static int flag = 0;
//    if(++flag == 2)
//    {
//        flag = 0;
//        ui->textEdit->insertPlainText(QString("\n"));
//        ui->textEdit->insertPlainText(qa);              //为什么出现了显示错误？因为这里出错了，用这个函数能够避免出错
//    }
//    else if(flag == 1)
//    {
//        ui->textEdit->insertPlainText(QString("   "));
//        ui->textEdit->insertPlainText(qa);
//    }
//    else
//    {
//        ui->textEdit->insertPlainText(qa);
//    }

    QList<QByteArray> read = qa.split(',');
    qa.clear();
    //qDebug() << "-----------" << read.size();

    //if(10 <= read.size())
    //{
        if(read.at(0)=="GC"&&read.at(9)=="R")
        //if(read.at(0)=="GC"&&read.at(7)=="R")
        {
            ui->lcdNumber->display(QString(read.at(1)));
            ui->lcdNumber_2->display(QString(read.at(2)));
            ui->lcdNumber_3->display(QString(read.at(3)));
            ui->lcdNumber_4->display(QString(read.at(4)));
            ui->lcdNumber_5->display(QString(read.at(5)));
            ui->lcdNumber_6->display(QString(read.at(6)));
            ui->lcdNumber_7->display(QString(read.at(7)));
            ui->lcdNumber_8->display(QString(read.at(8)));
        }

        QSqlQuery sql_query;
        QString insert_sql = "insert into sensor values (?, ?, ?, ?, ?, ?, ?)";
        sql_query.prepare(insert_sql);
        sql_query.addBindValue(current_date);
        sql_query.addBindValue(read.at(1));
        sql_query.addBindValue(read.at(2));
        sql_query.addBindValue(read.at(3));
        sql_query.addBindValue(read.at(4));
        sql_query.addBindValue(read.at(5));
        sql_query.addBindValue(read.at(6));

        if(!sql_query.exec())
        {
            qDebug() << sql_query.lastError();
        }
//        else
//        {
//            qDebug() << "inserted succeed!";
//        }

        if(1 == warnniing)
        {
            if(read.at(1).toInt() >= temperature)
            {
                emit signal_temperature();
            }

            if(read.at(2).toInt() >= humidity)
            {
                emit signal_humidity();
            }

            if(read.at(3).toInt() >= smoke)
            {
                emit signal_smoke();
            }

            if(read.at(4).toInt() >= air)
            {
                emit signal_air();
            }

            if(read.at(5).toInt() >= Illuminance)
            {
                emit signal_Illuminance();
            }

            if(read.at(6).toInt() >= fire)
            {
                emit signal_fire();
            }

            if(read.at(7).toInt() == 1)
            {
                emit signal_Person();
            }

            if(read.at(8).toInt() == 0)
            {
                emit signal_Rain();
            }
    //}
        }

    }
}

void MainWindow::on_pushButton_clicked()
{
    LoginWin *win = new LoginWin;//创建登录界面
    win->show();//显示登录界面
    delete this;//把主界面删除
}

void MainWindow::on_pushButton_2_clicked()
{
//    delete this;//把主界面删除

    //创建设置界面并且把this传入设置界面用于后面返回
    //注意MainWindow,SetWin界面在设计的时候（选择MainWiindow界面模板设计）实现
    SettingWin *swin = new SettingWin(this);
    swin->show();
    this->hide(); //主界面隐藏--后期直接可以显示
}

void MainWindow::on_pushButton_3_clicked()
{
    warnniing = 0;
    qDebug() << "warnniing:" << warnniing;
}

void MainWindow::on_pushButton_4_clicked()
{
    warnniing = 1;
    qDebug() << "warnniing" << warnniing;
}

void MainWindow::slots_temperature()
{
    static int count_temperature = 0;
    if(++count_temperature > 20)
    {
        count_temperature = 0;
        int msg = QMessageBox::warning(this,"温度警告"
                                           ,"<font size='10' color='red'>温度过高，正在为您开启空调...</font>"
                                           ,QMessageBox::Yes|QMessageBox::Cancel);

        switch(msg)
        {
            case QMessageBox::Yes:
              ui->label->setText(" 开启 空调...");
              break;
            case QMessageBox::Cancel:
              ui->label->setText(" 关闭 空调...");
              break;
            default:
              break;
        }
    }
}

void MainWindow::slots_humidity()
{
    static int count_humidity = 0;
    if(++count_humidity > 20)
    {
        qDebug() << "count_humidity:"<< count_humidity;
        count_humidity = 0;
        int msg = QMessageBox::warning(this,"湿度警告"
                                           ,"<font size='10' color='red'>湿度过高，正在为您开启抽湿机...</font>"
                                           ,QMessageBox::Yes|QMessageBox::Cancel);

        switch(msg)
        {
            case QMessageBox::Yes:
              ui->label->setText(" 开启 抽湿机...");
              break;
            case QMessageBox::Cancel:
              ui->label->setText(" 关闭 抽湿机...");
              break;
            default:
              break;
        }
    }
}

void MainWindow::slots_smoke()
{
    static int count_smoke = 0;
    if(++count_smoke > 20)
    {
        count_smoke = 0;
        int msg = QMessageBox::warning(this,"可燃气浓度警告"
                                           ,"<font size='10' color='red'>可燃气浓度过高，正在为您开启抽风机...</font>"
                                           ,QMessageBox::Yes|QMessageBox::Cancel);

        switch(msg)
        {
            case QMessageBox::Yes:
              ui->label->setText(" 开启 抽风机...");
              break;
            case QMessageBox::Cancel:
              ui->label->setText(" 关闭 抽风机...");
              break;
            default:
              break;
        }
    }
}

void MainWindow::slots_air()
{
    static int count_air = 0;
    if(++count_air > 20)
    {
        count_air = 0;
        int msg = QMessageBox::warning(this,"可燃气警告"
                                           ,"<font size='10' color='red'>可燃气浓度过高，正在为您开启抽风机...</font>"
                                           ,QMessageBox::Yes|QMessageBox::Cancel);

        switch(msg)
        {
            case QMessageBox::Yes:
              ui->label->setText(" 开启 抽风机...");
              break;
            case QMessageBox::Cancel:
              ui->label->setText(" 关闭 抽风机...");
              break;
            default:
              break;
        }
    }
}

void MainWindow::slots_Illuminance()
{
    static int count_Illuminance = 0;
    if(++count_Illuminance > 20)
    {
        count_Illuminance = 0;
        int msg = QMessageBox::warning(this,"亮度过低警告"
                                           ,"<font size='10' color='red'>亮度过低，正在为您开启壁灯...</font>"
                                           ,QMessageBox::Yes|QMessageBox::Cancel);

        switch(msg)
        {
            case QMessageBox::Yes:
              ui->label->setText(" 开启 壁灯...");
              break;
            case QMessageBox::Cancel:
              ui->label->setText(" 关闭 壁灯...");
              break;
            default:
              break;
        }
    }
}

void MainWindow::slots_fire()
{
    static int count_fire = 0;
    if(++count_fire > 20)
    {
        count_fire = 0;
        int msg = QMessageBox::warning(this,"火焰警告"
                                           ,"<font size='10' color='red'>火焰照度过高，正在为您开启报警...</font>"
                                           ,QMessageBox::Yes|QMessageBox::Cancel);

        switch(msg)
        {
            case QMessageBox::Yes:
              ui->label->setText(" 开启 报警...");
              break;
            case QMessageBox::Cancel:
              ui->label->setText(" 关闭 报警...");
              break;
            default:
              break;
        }
    }
}

void MainWindow::slots_Person()
{
    static int count_Person = 0;
    if(++count_Person > 5)
    {
        count_Person = 0;
        int msg = QMessageBox::warning(this,"闯入警告"
                                           ,"<font size='10' color='red'>人体闯入警告，正在为您开启报警...</font>"
                                           ,QMessageBox::Yes|QMessageBox::Cancel);

        switch(msg)
        {
            case QMessageBox::Yes:
              ui->label->setText(" 开启 报警...");
              break;
            case QMessageBox::Cancel:
              ui->label->setText(" 关闭 报警...");
              break;
            default:
              break;
        }
    }
}

void MainWindow::slots_Rain()
{
    static int count_Rain = 0;
    if(++count_Rain > 5)
    {
        count_Rain = 0;
        int msg = QMessageBox::warning(this,"雨水警告"
                                           ,"<font size='10' color='red'>监测到下雨，正在为您关闭窗户...</font>"
                                           ,QMessageBox::Yes|QMessageBox::Cancel);

        switch(msg)
        {
            case QMessageBox::Yes:
              ui->label->setText(" 关闭 窗户...");
              break;
            case QMessageBox::Cancel:
              ui->label->setText(" 开启 窗户...");
              break;
            default:
              break;
        }
    }
}
