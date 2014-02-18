#include "msgthread.h"

MsgThread::MsgThread(MainWindow *mainW,int port,QSqlDatabase *database,QObject *parent) :
    QThread(parent)
{
    mainWindow=mainW;
    listenPort=port;
    mTcpServer=new QTcpServer;
    mTcpServer->listen(QHostAddress::Any,port);
    dataBase=database;
    connect(mTcpServer,SIGNAL(newConnection()),this,SLOT(newConnect()));
}

void MsgThread::run()
{
}

void MsgThread::newConnect()
{

    mTcpSocket = mTcpServer->nextPendingConnection();
    subThread *mThread=new subThread(mainWindow,mTcpSocket,&mSocketList,dataBase);
    mThread->start();
}

