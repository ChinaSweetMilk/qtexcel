#include "loginwin.h"
#include "ui_loginwin.h"
#include "flatui.h"
#include "equipment.h"
#include <QDebug>
#include <QMessageBox>
#include <QIcon>
#include <QTime>
#include <QFile>
#include <QMap>
#include <QTime>
#include <QFileDialog>

LoginWin::LoginWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginWin)
{
    ui->setupUi(this);
    this->setWindowTitle("编码设计小工具");

    this->setWindowIcon(QIcon(":/new/prefix1/image/yayaya.jpg"));

    FlatUI::Instance()->setPushButtonQss(ui->btn1, 5, 8, "#1ABC9C", "#E6F8F5", "#2EE1C1", "#FFFFFF", "#16A086", "#A7EEE6");
    FlatUI::Instance()->setPushButtonQss(ui->btn2, 5, 8, "#3498DB", "#FFFFFF", "#5DACE4", "#E5FEFF", "#2483C7", "#A0DAFB");

    ui->lineEdit_xmdm->setAlignment(Qt::AlignCenter);
    ui->lineEdit_jddm->setAlignment(Qt::AlignCenter);
    ui->lineEdit_xmdm->setText(QString("01"));
    ui->lineEdit_jddm->setText(QString("01"));

    ui->comboBox_gclx->addItem("05-隧道机电工程");
    ui->comboBox_gclx->addItem("07-交通工程");

    //设置默认显示值的索引，从0开始
    ui->comboBox_gclx->setCurrentIndex(0);

    ui->comboBox_fxgc->addItem("02-隧道监控设施");
    ui->comboBox_fxgc->addItem("03-隧道照明设施");
    ui->comboBox_fxgc->addItem("04-隧道供配电设施");
    ui->comboBox_fxgc->addItem("05-隧道通风设施");
    ui->comboBox_fxgc->addItem("06-隧道消防设施");
    ui->comboBox_fxgc->addItem("07-隧道预留预埋");

    ui->comboBox_fxgc->setCurrentIndex(0);

    this->pexceloperator = new ExcelOperator;

    connect(ui->comboBox_gclx, SIGNAL(currentIndexChanged(int)), this, SLOT(GclxValueChange()));
}

LoginWin::~LoginWin()
{
    delete this->pexceloperator;
    delete ui;
}

// 读取配置文件按钮
void LoginWin::on_btn1_clicked()
{
    strfilename = QFileDialog::getOpenFileName(this, tr("选择配置文件！"),
                                                          ".",
                                                          tr("excel文件(*.xlsx)"));
    qDebug()<< "filename : " << QDir::toNativeSeparators(strfilename);

    // 判断是否已经选择配置文件
    if(strfilename.isEmpty())
    {
        QMessageBox::warning(this,QString(tr("警告")),
                                  QString(tr("请先选择配置文件！")));
        return;
    }

    this->pexceloperator->open(QDir::toNativeSeparators(strfilename));
    if(!this->pexceloperator->setSheet(1))
    {
        QMessageBox::warning(this,QString(tr("错误")),
                                  QString(tr("读取配置文件失败！")));
    }
    else
    {
        QMessageBox::information(NULL, "提示","成功读取配置文件", QMessageBox::Yes);
    }
}

//开始生成按钮
void LoginWin::on_btn2_clicked()
{
    // 获取项目代码、阶段代码的编号
    QString str_jddm = ui->lineEdit_jddm->text();
    QString str_xmdm = ui->lineEdit_xmdm->text();

    // 判断输入数据是否合法
    if(str_jddm.length() != 2 || str_xmdm.length() != 2)
    {
        QMessageBox::warning(this,QString(tr("警告")),
                                  QString(tr("项目代码或工程代码必须是两位！")));
        return;
    }

    // 判断是否已经选择配置文件
    if(strfilename.isEmpty())
    {
        QMessageBox::warning(this,QString(tr("警告")),
                                  QString(tr("请先选择配置文件！")));
        return;
    }

    // 获取项目工程类型、分项工程的名称
    QString str_gclx = ui->comboBox_gclx->currentText().split("-").at(1);
    QString str_fxgc = ui->comboBox_fxgc->currentText().split("-").at(1);

    // 拼接固定的ID前缀
    QString str_firstprefix;
    str_firstprefix =ui->lineEdit_xmdm->text() + "-" + ui->lineEdit_xmdm->text()
                                            + "-" + ui->comboBox_gclx->currentText().left(2)
                                            + "-" + ui->comboBox_fxgc->currentText().left(2)
                                            + ".";

    // 将拼接好的前缀输出到界面的输入框
    // ui->textEdit->clear();
    // ui->textEdit->insertPlainText(str_firstprefix);

    QTime time_StartTime = QTime::currentTime();


    QList<QList<QVariant> > listsheet;
    this->pexceloperator->readAll(listsheet);

    // 获取列数量
    int iColumnsCount = listsheet.at(0).size();
    // qDebug() << "iColumnsCount:" << iColumnsCount;

    // 获取设备数量
    int iRowsCount = listsheet.size() - 2;
    // qDebug() << "iRowsCount:" << iRowsCount;


    // 读取安装位置及安装位置编码
    QMap<QString, QString> mapPosition;

    // 安装位置数量
    int iPosCount = iColumnsCount - 5;
    for (int iColumnsPos = 0; iColumnsPos <= iPosCount; iColumnsPos++)
    {
        QString strInstallationPosition = (iColumnsPos > 9) ? QString::number(iColumnsPos) : ("0" + QString::number(iColumnsPos));
//        qDebug() << "listsheet.at(0).at(iColumnsPos + 4).toString():" << listsheet.at(0).at(iColumnsPos + 4).toString();
        mapPosition.insert(strInstallationPosition, listsheet.at(0).at(iColumnsPos + 4).toString());
    }

    // 创建Equipment对象
    Equipment * pEquipment = new Equipment[iRowsCount];
    unsigned int iSumIDCounts = 0;

    for (int iRowsPos = 0; iRowsPos < iRowsCount; iRowsPos++)
    {
        pEquipment[iRowsPos].setSubSystemSN(listsheet.at(iRowsPos + 2).at(0).toString());
        pEquipment[iRowsPos].setSubSystemName(listsheet.at(iRowsPos + 2).at(1).toString());
        pEquipment[iRowsPos].setEquipmentName(listsheet.at(iRowsPos + 2).at(2).toString());
        pEquipment[iRowsPos].setEquipmentSN(listsheet.at(iRowsPos + 2).at(3).toString());

        for (int iColumnsPos = 0; iColumnsPos <= iPosCount; iColumnsPos++)
        {
            QString strPosCounts = listsheet.at(iRowsPos + 2).at(iColumnsPos + 4).toString();
            if(strPosCounts.isEmpty())
            {
                continue;
            }

            QString strInstallationPosition = (iColumnsPos > 9) ? QString::number(iColumnsPos) : ("0" + QString::number(iColumnsPos));
            unsigned int iPosCounts = strPosCounts.toUInt();
            iSumIDCounts += iPosCounts;

            pEquipment[iRowsPos].intsertMapInstallationPosition(strInstallationPosition, iPosCounts);
        }
    }

    qDebug() << "iSumIDCounts:" << iSumIDCounts;

    this->pexceloperator->setEquipmentID(str_xmdm, str_jddm, str_gclx, str_fxgc, str_firstprefix, pEquipment, iRowsCount, mapPosition);

    // 释放Equipment对象
    delete [] pEquipment;


    // 关闭excel操作对象
    qDebug() << "++++++++++++++";
    this->pexceloperator->close();

    QTime time_StopTime = QTime::currentTime();
    int elapsed = time_StartTime.msecsTo(time_StopTime);
    QString strTimeSpent = "生成用时：" + QString::number(elapsed) + "ms";

    QMessageBox::information(NULL, "已生成全部ID", strTimeSpent, QMessageBox::Yes);
//    this->close();
}

//void LoginWin::proxyChange()
//{
//    //flag_huizhui = ui->comboBox->currentIndex();
////    qDebug() << "Now there are " << ui->comboBox->count() << "Items";
////    qDebug() << "The current item is" << ui->comboBox->currentText();
////    qDebug() << "The current item is" << ui->comboBox->currentIndex();
//    qDebug() << "The current flag_huizhui is" << flag_huizhui;
//}

void LoginWin::GclxValueChange()
{
    // 选择07-隧道机电工程
    if(ui->comboBox_gclx->currentIndex() == 1)
    {
        ui->comboBox_fxgc->clear();
        ui->comboBox_fxgc->addItem("03-监控设施");
        ui->comboBox_fxgc->addItem("04-收费设施");
        ui->comboBox_fxgc->addItem("05-通信设施");
        ui->comboBox_fxgc->addItem("06-供配电设施");
        ui->comboBox_fxgc->addItem("07-照明设施");
        ui->comboBox_fxgc->addItem("08-主线预埋工程");
    }
    // 选择05-隧道机电工程
    else
    {
        ui->comboBox_fxgc->clear();
        ui->comboBox_fxgc->addItem("02-隧道监控设施");
        ui->comboBox_fxgc->addItem("03-隧道照明设施");
        ui->comboBox_fxgc->addItem("04-隧道供配电设施");
        ui->comboBox_fxgc->addItem("05-隧道通风设施");
        ui->comboBox_fxgc->addItem("06-隧道消防设施");
        ui->comboBox_fxgc->addItem("07-隧道预留预埋");
    }
}
