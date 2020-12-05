#include "exceloperator.h"
#include <objbase.h>
#include <QDebug>

ExcelOperator::ExcelOperator(QObject *parent) : QObject(parent)
  , m_pExcel(NULL)
  , m_pWorkbooks(NULL)
  , m_pWorkbook(NULL)
  , m_pWorksheets(NULL)
  , m_pWorksheet(NULL)
{
    m_pExcel = new(std::nothrow) QAxObject();
    m_pExcel->setControl("Excel.Application");
    m_pExcel->dynamicCall("SetVisible(bool)", false); // true 表示操作文件时可见，false表示为不可见
    m_pExcel->setProperty("DisplayAlerts", false); // 设置不显示任何警告信息
    m_pWorkbooks = m_pExcel->querySubObject("WorkBooks");
}

ExcelOperator::~ExcelOperator()
{
//    close();
    m_pExcel->dynamicCall("Quit()");
    delete m_pExcel;
    m_pExcel = NULL;
}

bool ExcelOperator::open(QString strpath)
{
    m_strPath = strpath;
    CoInitializeEx(NULL, COINIT_MULTITHREADED); // 为当前线程初始化COM库并设置并发模式
    if (NULL == m_pExcel)
    {
        qCritical()<<"创建Excel对象失败...";
        return false;
    }
    try{
        // 按文件路径打开文件
        m_pWorkbook = m_pWorkbooks->querySubObject("Open(QString&)", m_strPath);

        // 获取打开的excel文件中所有的工作sheet
        m_pWorksheets = m_pWorkbook->querySubObject("WorkSheets");
    } catch (...){
        qCritical()<<"打开文件失败...";
        return false;
    }
    return true;
}

bool ExcelOperator::close()
{
    qDebug()<<"excel close...";
    if (m_pExcel)
    {
        qDebug()<<"closing...";
//        m_pWorkbook->dynamicCall("SaveAs(const QString&)", QDir::toNativeSeparators(m_strPath));
        m_pWorkbook->dynamicCall("Save()");
        m_pWorkbook->dynamicCall("Close()");
//        m_pExcel->dynamicCall("Quit()");
//        delete m_pExcel;
//        m_pExcel = NULL;
    }

    return true;
}

int ExcelOperator::getSheetsCount()
{
    int icount =  0;
    icount = m_pWorksheets->property("Count").toInt();
    return icount;
}

QString ExcelOperator::getPath()
{
    return m_strPath;
}

bool ExcelOperator::addSheet(QString strname)
{
    QAxObject *pWorkSheet = NULL;
    try{
        int icount = m_pWorksheets->property("Count").toInt();  //获取工作表数目

        QAxObject *pLastSheet = m_pWorksheets->querySubObject("Item(int)", icount);
        pWorkSheet = m_pWorksheets->querySubObject("Add(QVariant)", pLastSheet->asVariant());
        pLastSheet->dynamicCall("Move(QVariant)", pWorkSheet->asVariant());
        pWorkSheet->setProperty("Name", strname);  //设置工作表名称
        m_pWorkbook->dynamicCall("Save()");

        delete pWorkSheet;
        pWorkSheet = NULL;
    }
    catch (...){
        qCritical()<<"创建sheet失败...";
        return false;
    }

    if(pWorkSheet)
    {
        delete pWorkSheet;
        pWorkSheet = NULL;
    }
    return true;
}

bool ExcelOperator::delSheet(QString strname)
{
    try {
        QAxObject *pFirstSheet = m_pWorksheets->querySubObject("Item(QString)", strname);
        pFirstSheet->dynamicCall("delete");
    } catch (...) {
        qCritical()<<"删除sheet失败...";
        return false;
    }
    return true;
}

bool ExcelOperator::delSheet(int iindex)
{
    try {
        QAxObject *pFirstSheet = m_pWorksheets->querySubObject("Item(int)", iindex);
        pFirstSheet->dynamicCall("delete");
    } catch (...) {
        qCritical()<<"删除sheet失败...";
        return false;
    }
    return true;
}

bool ExcelOperator::setSheet(QString strname)
{
    try {
        m_pWorksheet = m_pWorksheet->querySubObject("Item(QString)", strname);
    } catch (...) {
        qCritical()<<"获取sheet失败...";
        return false;
    }
    return true;
}

bool ExcelOperator::setSheet(int iindex)
{
    // 获取当前工作簿的表数量
    int isheetcounts = this->getSheetsCount();

    // 如果要设置的表不在当前工作簿的范围，则创建该表
    if(iindex > isheetcounts)
    {
        QString strsheetname = "Sheet" + QString::number(iindex);
        if(!this->addSheet(strsheetname))
        {
            return false;
        }
    }

    try {
        m_pWorksheet = m_pWorksheets->querySubObject("Item(int)", iindex);
    } catch (...) {
        qCritical()<<"获取sheet失败...";
        return false;
    }
    return true;
}

QAxObject* ExcelOperator::getSheet(QString strname)
{
    QAxObject* pWorkSheet = NULL;
    try {
        pWorkSheet = m_pWorksheets->querySubObject("Item(QString)", strname);
    } catch (...) {
        qCritical()<<"获取sheet失败...";
    }
    return pWorkSheet;
}

QAxObject* ExcelOperator::getSheet(int iindex)
{
    QAxObject* pWorkSheet = NULL;
    try {
        pWorkSheet = m_pWorksheets->querySubObject("Item(int)", iindex);
    } catch (...) {
        qCritical()<<"获取sheet失败...";
    }
    return pWorkSheet;
}

QAxObject* ExcelOperator::getRows()
{
    QAxObject* pRows = NULL;
    try {
        QAxObject* pUsedRange = m_pWorksheet->querySubObject("UsedRange");
        pRows = pUsedRange->querySubObject("Rows");
    } catch (...) {
        qCritical()<<"获取行失败...";
    }
    return pRows;
}

int ExcelOperator::getRowsCount()
{
    int irows = 0;
    try {
        QAxObject* pUsedRange = m_pWorksheet->querySubObject("UsedRange");
        QAxObject* pRows = pUsedRange->querySubObject("Rows");
        irows = pRows->property("Count").toInt();
    } catch (...) {
        qCritical()<<"获取行数失败...";
        return 0;
    }
    return irows;
}

QAxObject* ExcelOperator::getColumns()
{
    QAxObject* pColumns = NULL;
    try {
        QAxObject* pUsedRange = m_pWorksheet->querySubObject("UsedRange");
        pColumns = pUsedRange->querySubObject("Columns");
    } catch (...) {
        qCritical()<<"获取列失败...";
    }
    return pColumns;
}

int ExcelOperator::getColumnsCount()
{
    int icolumns = 0;
    try {
        QAxObject* pUsedRange = m_pWorksheet->querySubObject("UsedRange");
        QAxObject* pColumns = pUsedRange->querySubObject("Columns");;
        icolumns = pColumns->property("Count").toInt();
    } catch (...) {
        qCritical()<<"获取列数失败...";
        return 0;
    }
    return icolumns;
}

QString ExcelOperator::getCell(int iirow, int icolumn)
{
    QString strCell = "";
    try {
        QAxObject* pCell = m_pWorksheet->querySubObject("Cells(int, int)", iirow, icolumn);
        strCell = pCell->property("Value").toString();
    } catch (...) {
        qCritical()<<"获取单元格信息失败...";
    }

    return strCell;
}

QString ExcelOperator::getCell(QString strnumber)
{
    QString strCell = "";
    try {
        QAxObject* pCell = m_pWorksheet->querySubObject("Range(QString)", strnumber);
        strCell = pCell->property("Value").toString();
    } catch (...) {
        qCritical()<<"获取单元格信息失败...";
    }

    return strCell;
}

bool ExcelOperator::setCell(int irow, int icolumn, QString strvalue)
{
    try {
        QAxObject* pCell = m_pWorksheet->querySubObject("Cells(int, int)", irow, icolumn);
        pCell->setProperty("Value", strvalue);
    } catch (...) {
        qCritical()<<"写入单元格信息失败...";
        return false;
    }
    return true;
}

bool ExcelOperator::setCell(QString strnumber, QString strvalue)
{
    try {
        QAxObject* pCell = m_pWorksheet->querySubObject("Range(QString)", strnumber);
        pCell->setProperty("Value", strvalue);
    } catch (...) {
        qCritical()<<"写入单元格信息失败...";
        return false;
    }
    return true;
}

bool ExcelOperator::setEquipmentID(QString strxmdm, QString strjddm, QString strgclx, QString strfxgc, QString strfirstprefix,
                                   Equipment * const pequipment, int iequipmentcounts, QMap<QString, QString> mapPosition)
{
    // 判断数据合法性
    if(strxmdm.isEmpty() || strjddm.isEmpty() || strgclx.isEmpty() || strfxgc.isEmpty() || strfirstprefix.isEmpty())
    {
        return false;
    }

    if(NULL == pequipment || 0 == iequipmentcounts || mapPosition.isEmpty())
    {
        return false;
    }

    if(NULL == m_pWorksheet)
    {
        qCritical()<<"Worksheet为空...";
        return false;
    }

    // 将当前工作表设置为表2
    if(!this->setSheet(2))
    {
        qDebug() << "setEquipmentID setSheet error";
        return false;
    }

    QList< QList<QVariant> > listIDdatas;

    // 遍历全部设备
    for(int iEquipmentPos = 0; iEquipmentPos < iequipmentcounts; iEquipmentPos++)
    {
        QMap<QString, unsigned int> mapEquipmentPosition = pequipment[iEquipmentPos].getMapInstallationPosition();
        QMap<QString, unsigned int>::Iterator it = mapEquipmentPosition.begin();

        while(it != mapEquipmentPosition.end())
        {
            // qDebug() << it.key() << "\t" << it.value();
            QString strPosition;
            QString strEquipmentIDPrefix;

            strEquipmentIDPrefix = strfirstprefix + pequipment[iEquipmentPos].getSubSystemSN() + it.key() + "."
                                             + pequipment[iEquipmentPos].getEquipmentSN() + ".";

            if(mapPosition.contains(it.key()))
            {
                // 获取安装位置的名称
                strPosition =  mapPosition.find(it.key()).value();
            }

            for(unsigned int iPosRows = 0; iPosRows < it.value(); iPosRows++)
            {
                // 生成四位设备ID，不足四位补0
                QString strEquipmentID = strEquipmentIDPrefix  + QString("%1").arg(iPosRows + 1, 4, 10, QChar('0'));

//                setCell(irowpos, 1, strxmdm);
//                setCell(irowpos, 2, strjddm);
//                setCell(irowpos, 3, strgclx);
//                setCell(irowpos, 4, strfxgc);

//                setCell(irowpos, 5, pequipment[iEquipmentPos].getSubSystemName());
//                setCell(irowpos, 6, strPosition);
//                setCell(irowpos, 7, pequipment[iEquipmentPos].getEquipmentName());

//                setCell(irowpos, 8, strEquipmentID);

                // 当前行内容
                QList<QVariant> listrows;

                listrows.append(strxmdm);
                listrows.append(strjddm);
                listrows.append(strgclx);
                listrows.append(strfxgc);
                listrows.append(pequipment[iEquipmentPos].getSubSystemName());
                listrows.append(strPosition);
                listrows.append(pequipment[iEquipmentPos].getEquipmentName());
                listrows.append(strEquipmentID);

                // 将这行内容添加到列表
                listIDdatas.append(listrows);
            }

            it++;
        }
    }

    // 将所有内容写入到sheet文件
    this->writeCurrentSheet(listIDdatas);

    return true;
}

void ExcelOperator::readAll(QList<QList<QVariant> > &res)
{
    QVariant var;

    if (m_pWorksheet != NULL && ! m_pWorksheet->isNull())
    {
        // 获取该sheet的数据范围
        QAxObject * ax_usedRange = m_pWorksheet->querySubObject("UsedRange");
        if(NULL == ax_usedRange || ax_usedRange->isNull())
        {
            return;
        }

        // 获取该sheet的数据内容
        var = ax_usedRange->dynamicCall("Value");
        delete ax_usedRange;
    }

    this->castVariant2ListListVariant(var, res);

    return;
}

bool ExcelOperator::writeCurrentSheet(const QList<QList<QVariant> > &cells)
{
    if(cells.size() <= 0)
        return false;

    if(NULL == this->m_pWorksheet || this->m_pWorksheet->isNull())
        return false;

    // 获取写入内容行数
    int irow = cells.size();
    // 获取写入内容列数
    int icol = cells.at(0).size();

    QString strRang;
    // 获取写入矩形内容最右边列标识
    this->convertToColName(icol, strRang);
    // 获取写入矩形内容最右边右下角单元格标识
    strRang += QString::number(irow);
    // 获取写入矩形内容写入范围
    strRang = "A2:" + strRang;

    qDebug()<< strRang;
    QAxObject *range = this->m_pWorksheet->querySubObject("Range(const QString&)", strRang);

    if(NULL == range || range->isNull())
    {
        return false;
    }

    bool succ = false;

    QVariant var;
    this->castListListVariant2Variant(cells, var);

    // 将数据写入到sheet
    succ = range->setProperty("Value", var);

    delete range;
    return succ;
}

void ExcelOperator::castVariant2ListListVariant(const QVariant &var, QList<QList<QVariant> > &res)
{
    QVariantList varRows = var.toList();

    if(varRows.isEmpty())
    {
        return;
    }

    const int rowCount = varRows.size();
    QVariantList rowData;

    for(int i=0;i<rowCount;++i)
    {
        rowData = varRows[i].toList();
        res.push_back(rowData);
    }
}

void ExcelOperator::castListListVariant2Variant(const QList<QList<QVariant> > &listcells, QVariant &varres)
{
    QVariantList vars;
    const int rows = listcells.size();
    for(int i = 0; i < rows; ++i)
    {
        vars.append(QVariant(listcells[i]));
    }
    varres = QVariant(vars);
}

void ExcelOperator::convertToColName(int idata, QString &strres)
{
    Q_ASSERT(idata > 0 && idata<65535);
    int tempData = idata / 26;
    if(tempData > 0)
    {
        int mode = idata % 26;
        this->convertToColName(mode, strres);
        this->convertToColName(tempData, strres);
    }
    else
    {
        strres = (to26AlphabetString(idata)+ strres);
    }
}

// 数字转换为26字母
QString ExcelOperator::to26AlphabetString(int idata)
{
    QChar ch = idata + 0x40; // A对应0x41
    return QString(ch);
}
