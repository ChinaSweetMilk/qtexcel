#ifndef EXCELOPERATOR_H
#define EXCELOPERATOR_H

#include <QObject>
#include <ActiveQt/QAxObject>
#include <QDebug>
#include <QDir>
#include <QMap>
#include "equipment.h"

class ExcelOperator : public QObject
{
    Q_OBJECT
public:
    explicit ExcelOperator(QObject *parent = nullptr);
    ~ExcelOperator();
    //打开文件
    bool open(QString strpath);

    //关闭文件
    bool close();

    // 获取工作表数量
    int getSheetsCount();

    // 获取工作表路径
    QString getPath();

    // 根据名称创建工作表
    bool addSheet(QString strname);

    // 根据名称删除工作表
    bool delSheet(QString strname);

    //根据编号删除工作表
    bool delSheet(int iindex);

    // 根据名称设置工作表
    bool setSheet(QString strname);

    // 根据序号设置工作表
    bool setSheet(int iindex);

    //根据名称获取工作表
    QAxObject* getSheet(QString strname);

    //根据编号获取工作表
    QAxObject* getSheet(int iindex);

    //获取行对象
    QAxObject* getRows();

    //获取行数
    int getRowsCount();

    //获取列对象
    QAxObject* getColumns();

    //获取列数
    int getColumnsCount();

    //根据行列值获取单元格值, 如: 3行，5列
    QString getCell(int irow, int icolumn);

    //根据行列编号获取单元格值, 如: "F6"
    QString getCell(QString strnumber);

    //根据行列值设置单元格值
    bool setCell(int irow, int icolumn, QString strvalue);

    //根据行列编号设置单元格值
    bool setCell(QString strnumber, QString strvalue);

    // 创建ID
    bool setEquipmentID(QString strjddm, QString strxmdm, QString strgclx, QString strfxgc, QString strfirstprefix,
                        Equipment * const pequipment,int iequipmentcounts, QMap<QString, QString> mapPosition);

    // 一次读取sheet的全部内容
    void readAll(QList<QList<QVariant> > &res);

    // 一次写入sheet的全部内容
    bool writeCurrentSheet(const QList<QList<QVariant> > &cells);

    // 把QVariant转为QList<QList<QVariant>>
    void castVariant2ListListVariant(const QVariant &var, QList<QList<QVariant> > &res);

    // 把QList<QList<QVariant>>转为QVariant
    void castListListVariant2Variant(const QList<QList<QVariant> > &cells, QVariant &res);

    // 把列数转换为excel的字母列号
    void convertToColName(int idata, QString &res);

    // 数字转换为26字母
    QString to26AlphabetString(int idata);

signals:

public slots:
private:
    QAxObject*      m_pExcel;
    QAxObject*      m_pWorkbooks;
    QAxObject*      m_pWorkbook;
    QAxObject*      m_pWorksheets;
    QAxObject*      m_pWorksheet;
    QString         m_strPath;
};
#endif // EXCELOPERATOR_H
