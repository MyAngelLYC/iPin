#ifndef MSGTHREAD_H
#define MSGTHREAD_H

#include "mainwindow.h"
#include "subthread.h"
#include <QThread>
#include <QtNetwork>
#include <QTextStream>
#include <QtSql>
#include <QMap>
#include <QVector>
class MsgThread : public QThread
{
    Q_OBJECT
public:
    explicit MsgThread(MainWindow *mainW,int port,QSqlDatabase*,QObject *parent = 0);

signals:

public slots:

public:
    bool isFirst=false;
    MainWindow *mainWindow;
    QTcpServer *mTcpServer;
    QTcpSocket *mTcpSocket;
    int listenPort;
    QMap<QString,QTcpSocket*> mSocketList;
    void run();
    QSqlDatabase *dataBase;
private slots:
    void newConnect();

};

#endif // MSGTHREAD_H
