#include "timethread.h"

timeThread::timeThread(QSqlDatabase *database,QMap<QString,QTcpSocket*> *SocketList,QObject *parent) :
    QThread(parent)
{/*
    dataBase=QSqlDatabase::addDatabase("QMYSQL");
    dataBase.setHostName("localhost");
    dataBase.setDatabaseName("iPing");
    dataBase.setUserName("root");
    dataBase.setPassword("123");
    dataBase.open();*/
    dataBase=database;
    sqlQuery=new QSqlQuery(dataBase->database());
    sqlCmd=QString("SELECT * FROM OnlineUser");
    mSocketList=SocketList;
    startTimer(10000);
}

void timeThread::timerEvent(QTimerEvent *event)
{
    sqlQuery->exec(sqlCmd);
    QSqlQuery tempQuery(dataBase->database());
    qint8 infoCount=sqlQuery->size();
    if(infoCount>0)
    {
        QString ID;
        int Online;
        for(int i=0;i<infoCount;i++)
        {
            sqlQuery->seek(i);
            ID = sqlQuery->value("ID").toString();
            Online = sqlQuery->value("Online").toInt();
              if(0==Online)
            {
                tempQuery.exec(QString("DELETE FROM OnlineUser WHERE ID='%1'").arg(ID));
                infoCount--;
                mSocketList->remove(ID);
            }
            else
            {
                tempQuery.exec(QString("UPDATE OnlineUser SET Online='0' WHERE ID='%1'").arg(ID));
            }
        }
    }
}
