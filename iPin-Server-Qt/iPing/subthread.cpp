#include "subthread.h"

subThread::subThread(MainWindow *mainW,QTcpSocket *mSocket,QMap<QString,QTcpSocket*> *SocketList,QSqlDatabase *database,QObject *parent) :
    QThread(parent)
{
    mainWindow=mainW;
    mTcpSocket=mSocket;
    mSocketList=SocketList;
    dataBase=database;
    connect(mTcpSocket,SIGNAL(readyRead()),this,SLOT(processPendingDatagram()));
}

void subThread::run()
{

}

void subThread::processPendingDatagram()
{
    QByteArray datagram = mTcpSocket->readAll();
    mainWindow->SetLineText(datagram);

    QTextStream dataStream(&datagram, QIODevice::ReadOnly);
    QString cmd;
    dataStream >> cmd;

    QSqlQuery sqlQuery(dataBase->database());
    QString sqlCmd;
    //if(listenPort==20000)
    //{
        if(cmd.contains("ServiceConnect"))
        {
            QString IP=mTcpSocket->peerAddress().toString();
            QString ID,username;
            dataStream >> ID >> username;
            sqlCmd=QString("SELECT * FROM OnlineUser WHERE ID='%1'").arg(ID);
            sqlQuery.exec(sqlCmd);
            qint8 infoCount=sqlQuery.size();
            if(infoCount<=0)
            {
                sqlCmd=QString("INSERT INTO OnlineUser (ID,username,IP_Address,Online,Longitude,Latitude,Destination) VALUES ('%1','%2','%3','1','-','-','-')").arg(ID).arg(username).arg(IP);
            }
            else
            {
                sqlCmd=QString("UPDATE OnlineUser SET IP_Address='%1',Online='1' WHERE ID='%2'").arg(IP).arg(ID);
            }
            sqlQuery.exec(sqlCmd);

            sqlCmd=QString("SELECT * FROM GroupList WHERE MemberList LIKE '%%1%'").arg(username);
            sqlQuery.exec(sqlCmd);
            infoCount=sqlQuery.size();
            for(int i=0;i<infoCount;i++)
            {
                sqlQuery.seek(i);
                QString GroupID=sqlQuery.value("GroupID").toString();
                QSqlQuery tempQuery(dataBase->database());
                tempQuery.exec(QString("SELECT * FROM GroupMessage%1 WHERE UnsendList LIKE '%%2%'").arg(GroupID).arg(username));
                qint8 msgCount=tempQuery.size();
                for(int i=0;i<msgCount;i++)
                {
                    tempQuery.seek(i);
                    int MsgNo;
                    QString Sender,date,message,unsendList;
                    MsgNo=tempQuery.value("MsgNo").toInt();
                    Sender=tempQuery.value("Sender").toString();
                    date=tempQuery.value("Date").toString();
                    message=tempQuery.value("Message").toString();
                    unsendList=tempQuery.value("UnsendList").toString();
                    QByteArray block;
                    block.append("get_msg`");
                    block.append(GroupID+"`");
                    block.append(Sender+"`");
                    block.append(date+"`");
                    block.append(message+"`");
                    block.append("oldMsg`");
                    mTcpSocket->write(block);
                    mTcpSocket->flush();
                    unsendList.replace(username+"|","");
                    sqlCmd=QString("UPDATE GroupMessage%1 SET UnsendList='%2' WHERE MsgNo='%3'").arg(GroupID).arg(unsendList).arg(MsgNo);
                    QSqlQuery tempQuery2(dataBase->database());
                    tempQuery2.exec(sqlCmd);
                    sleep(1);
                }
            }
            mSocketList->remove(ID);
            mSocketList->insert(ID,mTcpSocket);
        }
        else if(cmd.contains("send_msg"))
        {
            QString tempString=dataStream.readAll();
            QStringList tempList = tempString.split("`");
            QString GroupID,username,date,message;
            //dataStream>>GroupID>>username>>date>>message;
            GroupID=tempList[0];
            GroupID.replace(" ","");
            username=tempList[1];
            date=tempList[2];
            message=tempList[3];
            sqlCmd=QString("SELECT * FROM GroupList WHERE GroupID='%1'").arg(GroupID);
            sqlQuery.exec(sqlCmd);
            sqlQuery.last();
            QString memberList;
            int memberCount;
            memberCount=sqlQuery.value("MemberCount").toInt()-1;
            memberList=sqlQuery.value("MemberList").toString();
            memberList.replace(username+"|","");
            sqlQuery.exec(QString("SELECT * FROM GroupMessage%1").arg(GroupID));
            int count=sqlQuery.size();
            int MsgNo=1;
            if(count>0)
            {
                sqlQuery.last();
                MsgNo=sqlQuery.value("MsgNo").toInt()+1;
            }
            sqlCmd=QString("INSERT INTO GroupMessage%1 (MsgNo,Sender,Date,Message,UnsendList) VALUES('%2','%3','%4','%5','%6')").arg(GroupID).arg(MsgNo).arg(username).arg(date).arg(message).arg(memberList);
            sqlQuery.exec(sqlCmd);
            QStringList temp=memberList.split("|");
            for(int i=0;i<memberCount;i++)
            {
                sqlCmd=QString("SELECT * FROM OnlineUser WHERE username='%1'").arg(temp[i]);
                sqlQuery.exec(sqlCmd);
                qint8 infoCount=sqlQuery.size();
                if(temp[i]!=username && infoCount>0)
                {
                    QString ID;
                    sqlCmd=QString("SELECT * FROM user WHERE username='%1'").arg(temp[i]);
                    sqlQuery.exec(sqlCmd);
                    sqlQuery.last();
                    ID=sqlQuery.value("ID").toString();                    
                    QByteArray block;
                    block.append("get_msg`");
                    block.append(GroupID+"`");
                    block.append(username+"`");
                    block.append(date+"`");
                    block.append(message+"`");
                    block.append("liveMsg`");
                    QTcpSocket *mSocket=(QTcpSocket*)((*mSocketList)[ID]);
                    if(NULL!=mSocket)
                    {
                        mSocket->write(block);
                        mSocket->flush();
                        memberList.replace(temp[i]+"|","");
                    }

                }
            }
            sqlCmd=QString("UPDATE GroupMessage%1 SET UnsendList='%2' WHERE MsgNo='%3'").arg(GroupID).arg(memberList).arg(MsgNo);
            sqlQuery.exec(sqlCmd);
        }
        else if(cmd.contains("alive"))
        {
            QString ID;
            dataStream>>ID;

            sqlCmd=QString("UPDATE OnlineUser SET Online='1' WHERE ID='%1'").arg(ID);
            sqlQuery.exec(sqlCmd);
        }
        else if(cmd.contains("DischargeGroup"))
        {
            QString GroupID;
            dataStream>>GroupID;
            sqlCmd=QString("SELECT * FROM GroupList WHERE GroupID='%1'").arg(GroupID);
            sqlQuery.exec(sqlCmd);
            sqlQuery.last();
            QString from=sqlQuery.value("fromplace").toString();
            QString to=sqlQuery.value("toplace").toString();
            QString memberList=sqlQuery.value("MemberList").toString();
            int memberCount=sqlQuery.value("MemberCount").toInt();
            QStringList temp=memberList.split("|");
            for(int i=0;i<memberCount;i++)
            {
                sqlCmd=QString("SELECT * FROM OnlineUser WHERE username='%1'").arg(temp[i]);
                sqlQuery.exec(sqlCmd);
                qint8 infoCount=sqlQuery.size();
                if(infoCount>0)
                {
                    QString ID;
                    sqlCmd=QString("SELECT * FROM user WHERE username='%1'").arg(temp[i]);
                    sqlQuery.exec(sqlCmd);
                    sqlQuery.last();
                    ID=sqlQuery.value("ID").toString();
                    QByteArray block;
                    block.append("dischargeGroup`");
                    block.append(GroupID+"`");
                    block.append(from+"`");
                    block.append(to+"`");
                    QTcpSocket *mSocket=(QTcpSocket*)((*mSocketList)[ID]);
                    if(NULL!=mSocket)
                    {
                        mSocket->write(block);
                        mSocket->flush();
                    }
                }
            }
            sqlCmd=QString("UPDATE GroupList SET MemberCount='0',MemberList='Empty' WHERE GroupID='%1'").arg(GroupID);
            sqlQuery.exec(sqlCmd);
            sqlCmd=QString("DROP TABLE GroupMessage%1").arg(GroupID);
            sqlQuery.exec(sqlCmd);
        }
        else if(cmd.contains("ExitGroup"))
        {
            QString GroupID,username;
            dataStream >> GroupID >> username;
            sqlCmd=QString("SELECT * FROM GroupList WHERE GroupID='%1'").arg(GroupID);
            sqlQuery.exec(sqlCmd);
            sqlQuery.last();
            QString memberList=sqlQuery.value("MemberList").toString();
            int memberCount=sqlQuery.value("MemberCount").toInt();
            QStringList temp=memberList.split("|");
            for(int i=0;i<memberCount;i++)
            {
                sqlCmd=QString("SELECT * FROM OnlineUser WHERE username='%1'").arg(temp[i]);
                sqlQuery.exec(sqlCmd);
                qint8 infoCount=sqlQuery.size();
                if(temp[i]!=username && infoCount>0)
                {
                    QString ID;
                    sqlCmd=QString("SELECT * FROM user WHERE username='%1'").arg(temp[i]);
                    sqlQuery.exec(sqlCmd);
                    sqlQuery.last();
                    ID=sqlQuery.value("ID").toString();
                    QByteArray block;
                    block.append("exitGroup`");
                    block.append(GroupID+"`");
                    block.append(username+"`");
                    QTcpSocket *mSocket=(QTcpSocket*)((*mSocketList)[ID]);
                    if(NULL!=mSocket)
                    {
                        mSocket->write(block);
                        mSocket->flush();
                    }
                }
            }
            memberList.replace(username+"|","");
            memberCount-=1;
            sqlCmd=QString("UPDATE GroupList SET MemberCount='%1',MemberList='%2' WHERE GroupID='%3'").arg(memberCount).arg(memberList).arg(GroupID);
            sqlQuery.exec(sqlCmd);
        }
        else if(cmd.contains("DeleteMember"))
        {
            QString GroupID,username;
            dataStream >> GroupID >> username;
            sqlCmd=QString("SELECT * FROM GroupList WHERE GroupID='%1'").arg(GroupID);
            sqlQuery.exec(sqlCmd);
            sqlQuery.last();
            QString from=sqlQuery.value("fromplace").toString();
            QString to=sqlQuery.value("toplace").toString();
            QString memberList=sqlQuery.value("MemberList").toString();
            int memberCount=sqlQuery.value("MemberCount").toInt();
            QStringList temp=memberList.split("|");
            for(int i=0;i<memberCount;i++)
            {
                sqlCmd=QString("SELECT * FROM OnlineUser WHERE username='%1'").arg(temp[i]);
                sqlQuery.exec(sqlCmd);
                qint8 infoCount=sqlQuery.size();
                if(infoCount>0)
                {
                    QString ID;
                    sqlCmd=QString("SELECT * FROM user WHERE username='%1'").arg(temp[i]);
                    sqlQuery.exec(sqlCmd);
                    sqlQuery.last();
                    ID=sqlQuery.value("ID").toString();
                    QByteArray block;
                    block.append("deleteMember`");
                    block.append(GroupID+"`");
                    block.append(username+"`");
                    block.append(from+"`");
                    block.append(to+"`");
                    QTcpSocket *mSocket=(QTcpSocket*)((*mSocketList)[ID]);
                    if(NULL!=mSocket)
                    {
                        mSocket->write(block);
                        mSocket->flush();
                    }
                }
            }
            memberList.replace(username+"|","");
            memberCount-=1;
            sqlCmd=QString("UPDATE GroupList SET MemberCount='%1',MemberList='%2' WHERE GroupID='%3'").arg(memberCount).arg(memberList).arg(GroupID);
            sqlQuery.exec(sqlCmd);
        }
        else if(cmd.contains("JoinGroup"))
        {
            QString GroupID,username;
            dataStream >> GroupID >> username;
            sqlCmd=QString("SELECT * FROM GroupList WHERE GroupID='%1'").arg(GroupID);
            sqlQuery.exec(sqlCmd);
            sqlQuery.last();
            QString memberList=sqlQuery.value("MemberList").toString();
            int memberCount=sqlQuery.value("MemberCount").toInt();
            QStringList temp=memberList.split("|");
            for(int i=0;i<memberCount;i++)
            {
                sqlCmd=QString("SELECT * FROM OnlineUser WHERE username='%1'").arg(temp[i]);
                sqlQuery.exec(sqlCmd);
                qint8 infoCount=sqlQuery.size();
                if(infoCount>0)
                {
                    QString ID;
                    sqlCmd=QString("SELECT * FROM user WHERE username='%1'").arg(temp[i]);
                    sqlQuery.exec(sqlCmd);
                    sqlQuery.last();
                    ID=sqlQuery.value("ID").toString();
                    QByteArray block;
                    block.append("joinGroup`");
                    block.append(GroupID+"`");
                    block.append(username+"`");
                    QTcpSocket *mSocket=(QTcpSocket*)((*mSocketList)[ID]);
                    if(NULL!=mSocket)
                    {
                        mSocket->write(block);
                        mSocket->flush();
                    }
                }
            }
            username=memberList+username+"|";
            memberCount+=1;
            sqlCmd=QString("UPDATE GroupList SET MemberCount='%1',MemberList='%2' WHERE GroupID='%3'").arg(memberCount).arg(username).arg(GroupID);
            sqlQuery.exec(sqlCmd);
            sendMessage("JoinGroupSuccess");
        }
}

void subThread::sendMessage(QString sendString)

{
    QByteArray block;
    block.append(sendString);
    mTcpSocket->write(block);
}
