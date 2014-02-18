#ifndef SUBTHREAD_H
#define SUBTHREAD_H

#include "mainwindow.h"
#include <QThread>
#include <QtNetwork>
#include <QTextStream>
#include <QtSql>
#include <QMap>
#include <QVector>
class subThread : public QThread
{
    Q_OBJECT
public:
    explicit subThread(MainWindow *mainW,QTcpSocket*,QMap<QString,QTcpSocket*>*,QSqlDatabase*,QObject *parent = 0);

signals:

public slots:

protected:
    bool isFirst=false;
    MainWindow *mainWindow;
    QTcpSocket *mTcpSocket;
    QMap<QString,QTcpSocket*> *mSocketList;
    void run();
    QSqlDatabase *dataBase;
private slots:
    void processPendingDatagram();
    void sendMessage(QString sendString);

};
#endif // SUBTHREAD_H
