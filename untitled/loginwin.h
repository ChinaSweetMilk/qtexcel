#ifndef LOGINWIN_H
#define LOGINWIN_H

#include <QMainWindow>
#include "exceloperator.h"

namespace Ui {
class LoginWin;
}

class LoginWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginWin(QWidget *parent = 0);
    ~LoginWin();
    int OpenDatabase();
private:
    Ui::LoginWin *ui;
private slots:
    void on_btn1_clicked();
    void on_btn2_clicked();
//    void proxyChange();
    void GclxValueChange();
//    void FxgcValueChange();
    void on_lineEdit_xmdm_textChanged(const QString &arg1);
    void on_comboBox_gclx_currentIndexChanged(const QString &arg1);

public:
    QString strfilename;

    // 操作excel文件
    ExcelOperator * pexceloperator;
};

#endif // LOGINWIN_H
