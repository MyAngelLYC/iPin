#ifndef MYTHREAD_H
#define MYTHREAD_H

#include "mainwindow.h"
#include <QThread>
#include <QtNetwork>
#include <QTextStream>
#include <QtSql>
class myThread : public QThread
{
    Q_OBJECT
public:
    explicit myThread(MainWindow *mainW,int port,QSqlDatabase*,QObject *parent = 0);
    
signals:
    
public slots:

protected:
    bool isFirst=false;
    MainWindow *mainWindow;
    QTcpServer *mTcpServer;
    QTcpSocket *mTcpSocket;
    int listenPort;
    qint64 totalBytes;
    qint64 ReceivedBytes;
    QFile *file;
    QSqlDatabase *dataBase;
    void run();

private slots:
    void newConnect();
    void processPendingDatagram();
    void sendMessage(QString sendString);
    
};

#endif // MYTHREAD_H
