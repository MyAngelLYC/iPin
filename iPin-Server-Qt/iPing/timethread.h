#ifndef TIMETHREAD_H
#define TIMETHREAD_H

#include <QThread>
#include <QtSql>
#include <QtNetwork>
class timeThread : public QThread
{
    Q_OBJECT
public:
    explicit timeThread(QSqlDatabase *,QMap<QString,QTcpSocket*> *,QObject *parent = 0);
    //QSqlDatabase dataBase;
    QSqlQuery *sqlQuery;
    QString sqlCmd;
    QMap<QString,QTcpSocket*> *mSocketList;
signals:
    
public slots:

protected:
    void timerEvent(QTimerEvent *);
    QSqlDatabase *dataBase;
};

#endif // TIMETHREAD_H
