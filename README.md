# qtexcel
qt利用QAxObjecti操作大数据excel

查阅了大部分资料，大数据读写excel文件用QAxObject对象最快，借鉴了一些读写excel文件的demo，所以本文也采用了这种方法。
读取单个单元格内容时，property("Value").toString()，一秒大概能读写200行左右。但是需要大量读写单元格时，以上方法将会变得十分缓慢，所以这时需要用到QVariant，一次读取或写入整个工作表。采用以下方法，能将效率大大提高，一万行大概500毫秒。这个版本还没采用多线程读写，后面加入效率还可以提高。

读写单元格时，常用以下这种方法：

// 根据行号、列号读取某个单元格内容
QAxObject* pCell = m_pWorksheet->querySubObject("Cells(int, int)", iirow, icolumn);
QString    strCell = pCell->property("Value").toString();

// 根据行列值读取某个单元格内容
QAxObject* pCell = pWorksheet->querySubObject("Range(QString)", strnumber);
QString    strCell = pCell->property("Value").toString();

// 根据行号、列号写入某个单元格内容
QAxObject* pCell = m_pWorksheet->querySubObject("Cells(int, int)", irow, icolumn);
QString    pCell->setProperty("Value", strvalue);

// 根据行列值读取某个单元格内容
QAxObject* pCell = m_pWorksheet->querySubObject("Range(QString)", strnumber);
QString    pCell->setProperty("Value", strvalue);


以下是一次读取工作表全部内容的方法：

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

以下是一次写入工作表全部内容的方法：
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
    convertToColName(icol, strRang);
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
    castListListVariant2Variant(cells,var);

    // 将数据写入到sheet
    succ = range->setProperty("Value", var);

    delete range;
    return succ;
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
        convertToColName(mode, strres);
        convertToColName(tempData, strres);
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


一、简单应用demo

