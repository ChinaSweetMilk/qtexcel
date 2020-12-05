#include "settingwin.h"
#include "ui_settingwin.h"
#include "mainwindow.h"
#include "flatui.h"
#include <QDebug>
#include "qdesktopwidget.h"
#include "qdatetime.h"
#include <QIcon>


SettingWin::SettingWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SettingWin)
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

    if(database.tables().contains("setting"))
    {
        qDebug() << "setting 表存在";
    }
    else
    {
        qDebug() << "setting 表不存在！！";
    }

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

    this->initForm();
}

SettingWin::~SettingWin()
{
    delete ui;
}

void SettingWin::initForm()
{
    ui->bar1->setRange(0, 100);
    ui->bar2->setRange(0, 100);
    ui->bar3->setRange(0, 100);
    ui->bar4->setRange(0, 100);
    ui->bar5->setRange(0, 100);
    ui->bar6->setRange(0, 100);

    ui->slider1->setRange(0, 100);
    ui->slider2->setRange(0, 100);
    ui->slider3->setRange(0, 100);
    ui->slider4->setRange(0, 100);
    ui->slider5->setRange(0, 100);
    ui->slider6->setRange(0, 100);

    connect(ui->slider1, SIGNAL(valueChanged(int)), ui->bar1, SLOT(setValue(int)));
    connect(ui->slider1, SIGNAL(valueChanged(int)), this, SLOT(UpdateTemp(int)));

    connect(ui->slider2, SIGNAL(valueChanged(int)), ui->bar2, SLOT(setValue(int)));
    connect(ui->slider2, SIGNAL(valueChanged(int)), this, SLOT(UpdateHum(int)));

    connect(ui->slider3, SIGNAL(valueChanged(int)), ui->bar3, SLOT(setValue(int)));
    connect(ui->slider3, SIGNAL(valueChanged(int)), this, SLOT(UpdateSmoke(int)));

    connect(ui->slider4, SIGNAL(valueChanged(int)), ui->bar4, SLOT(setValue(int)));
    connect(ui->slider4, SIGNAL(valueChanged(int)), this, SLOT(UpdateAir(int)));

    connect(ui->slider5, SIGNAL(valueChanged(int)), ui->bar5, SLOT(setValue(int)));
    connect(ui->slider5, SIGNAL(valueChanged(int)), this, SLOT(UpdateIllum(int)));

    connect(ui->slider6, SIGNAL(valueChanged(int)), ui->bar6, SLOT(setValue(int)));
    connect(ui->slider6, SIGNAL(valueChanged(int)), this, SLOT(UpdateFire(int)));

    ui->slider1->setValue(temperature);
    ui->slider2->setValue(humidity);
    ui->slider3->setValue(smoke);
    ui->slider4->setValue(air);
    ui->slider5->setValue(Illuminance);
    ui->slider6->setValue(fire);

    this->setStyleSheet("*{outline:0px;}QWidget#frmFlatUI{background:#FFFFFF;}");

    FlatUI::Instance()->setPushButtonQss(ui->pushButton, 5, 8, "#1ABC9C", "#E6F8F5", "#2EE1C1", "#FFFFFF", "#16A086", "#A7EEE6");

    FlatUI::Instance()->setProgressBarQss(ui->bar1, 15, 5, 12, "#E8EDF2", "#FF0000");
    FlatUI::Instance()->setProgressBarQss(ui->bar2, 15, 5, 12, "#E8EDF2", "#FF8800");
    FlatUI::Instance()->setProgressBarQss(ui->bar3, 15, 5, 12, "#E8EDF2", "#FFFF00");
    FlatUI::Instance()->setProgressBarQss(ui->bar4, 15, 5, 12, "#E8EDF2", "#77FF00");
    FlatUI::Instance()->setProgressBarQss(ui->bar5, 15, 5, 12, "#E8EDF2", "#77FFCC");
    FlatUI::Instance()->setProgressBarQss(ui->bar6, 15, 5, 12, "#E8EDF2", "#00FFFF");

    FlatUI::Instance()->setSliderQss(ui->slider1, 5, "#E8EDF2", "#34495E", "#34495E");
    FlatUI::Instance()->setSliderQss(ui->slider2, 5, "#E8EDF2", "#34495E", "#34495E");
    FlatUI::Instance()->setSliderQss(ui->slider3, 5, "#E8EDF2", "#34495E", "#34495E");
    FlatUI::Instance()->setSliderQss(ui->slider4, 5, "#E8EDF2", "#34495E", "#34495E");
    FlatUI::Instance()->setSliderQss(ui->slider5, 5, "#E8EDF2", "#34495E", "#34495E");
    FlatUI::Instance()->setSliderQss(ui->slider6, 5, "#E8EDF2", "#34495E", "#34495E");
}

void SettingWin::UpdateTemp(int val)
{
  temperature = val;
  qDebug()<<"====="<<temperature;
  qDebug()<<"成功------"<<val;
}

void SettingWin::UpdateHum(int val)
{
  humidity = val;
}

void SettingWin::UpdateSmoke(int val)
{
  smoke = val;
}

void SettingWin::UpdateAir(int val)
{
  air = val;
}

void SettingWin::UpdateIllum(int val)
{
  Illuminance = val;
}

void SettingWin::UpdateFire(int val)
{
  fire = val;
}

void SettingWin::on_pushButton_clicked()
{
    QSqlQuery sql_query;
    QString delete_sql = "delete from setting";
    if(!sql_query.exec(delete_sql))
    {
        qDebug() << sql_query.lastError();
    }
    else
    {
        qDebug() << "delete!";
    }

    QString insert_sql = "insert into setting values (?, ?, ?, ?, ?, ?)";
    sql_query.prepare(insert_sql);
    sql_query.addBindValue(temperature);
    sql_query.addBindValue(humidity);
    sql_query.addBindValue(smoke);
    sql_query.addBindValue(air);
    sql_query.addBindValue(Illuminance);
    sql_query.addBindValue(fire);

    if(!sql_query.exec())
    {
        qDebug() << sql_query.lastError();
    }
    else
    {
        qDebug() << "inserted succeed!";
    }

    database.close();
//    this->parentWidget()->show();
    MainWindow *win = new MainWindow;
    win->show();
    this->close();//这里不能用delete，因为this是main函数中创建的栈空间系统自动释放

   // delete this;//释放设置界面 也可以用this->deleteLater()但是这个函数不会立即释放空间
}
