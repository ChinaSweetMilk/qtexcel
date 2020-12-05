#include <QApplication>
#include "loginwin.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

    LoginWin w;
    w.show();

    return a.exec();
}
