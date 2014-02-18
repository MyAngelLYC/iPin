#include "mainwindow.h"
#include "mythread.h"
#include "msgthread.h"
#include "timethread.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    QSqlDatabase dataBase=QSqlDatabase::addDatabase("QMYSQL");
    dataBase.setHostName("localhost");
    dataBase.setDatabaseName("iPing");
    dataBase.setUserName("root");
    dataBase.setPassword("123");
    dataBase.open();
    myThread thread(&w,19999,&dataBase);
    thread.start();
    MsgThread thread1(&w,20000,&dataBase);
    thread1.start();
    timeThread timethread(&dataBase,&(thread1.mSocketList));
    timethread.start();
    return a.exec();
}
