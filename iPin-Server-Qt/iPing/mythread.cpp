#include "mythread.h"

myThread::myThread(MainWindow *mainW,int port,QSqlDatabase *database,QObject *parent) :
    QThread(parent)
{
    mainWindow=mainW;
    listenPort=port;
    mTcpServer=new QTcpServer;
    mTcpServer->listen(QHostAddress::Any,port);
    totalBytes=0;
    ReceivedBytes=0;
    dataBase=database;
    connect(mTcpServer,SIGNAL(newConnection()),this,SLOT(newConnect()));
}

void myThread::run()
{
    if(!isFirst)
    {
        mainWindow->SetLineText("Start!");
        isFirst=true;
    }

}

void myThread::newConnect()
{

    mTcpSocket = mTcpServer->nextPendingConnection();
    connect(mTcpSocket,SIGNAL(readyRead()),this,SLOT(processPendingDatagram()));
}

void myThread::processPendingDatagram()
{
    QByteArray datagram = mTcpSocket->readAll();
    mainWindow->SetLineText(datagram);

    QTextStream dataStream(&datagram, QIODevice::ReadOnly);
    QString cmd;
    QString username;
    QString password;
    dataStream >> cmd;
    /*
    QSqlDatabase dataBase=QSqlDatabase::addDatabase("QMYSQL");
    dataBase.setHostName("localhost");
    dataBase.setDatabaseName("iPing");
    dataBase.setUserName("root");
    dataBase.setPassword("123");
    dataBase.open();*/
    QSqlQuery sqlQuery(dataBase->database());
    QString sqlCmd;
    if(listenPort==19999)
    {
        if(cmd.contains("userLogin"))
        {
            dataStream >> username >> password;
            sqlCmd=QString("SELECT * FROM user WHERE username = '%1' and password = '%2'").arg(username).arg(password);
            sqlQuery.exec(sqlCmd);
            sqlQuery.last();
            if(sqlQuery.at()<0)
                sendMessage("userLoginFaliure");
            else
            {
                QString ID = sqlQuery.value("ID").toString();
                QString sex = sqlQuery.value("sex").toString();
                QString telephone = sqlQuery.value("telephone").toString();
                QString HeadImageVersion = sqlQuery.value("HeadImageVersion").toString();
                sendMessage(QString("userLoginSuccess:%1:%2:%3:%4").arg(ID).arg(sex).arg(telephone).arg(HeadImageVersion));
            }
        }
        else if(cmd.contains("userRegister"))
        {
            dataStream >> username >> password;
            sqlCmd=QString("SELECT * FROM user WHERE username = '%1'").arg(username);
            sqlQuery.exec(sqlCmd);
            sqlQuery.last();
            if(sqlQuery.at()>=0)
                sendMessage("userRegisterFaliure");
            else
            {                
                QString sex;
                QString telephone;
                dataStream >> sex >> telephone;
                sqlCmd=QString("SELECT * FROM user");
                sqlQuery.exec(sqlCmd);
                sqlQuery.last();
                int No = sqlQuery.at();
                if(No>=0)
                {
                    No+=2;
                }
                else
                {
                    No=1;
                }
                QString ID;
                ID.sprintf("%09d",No);
                sqlCmd=QString("INSERT INTO user (ID,username,password,sex,telephone,HeadImageVersion) VALUES ('%1','%2','%3','%4','%5','%6')").arg(ID).arg(username).arg(password).arg(sex).arg(telephone).arg("0");
                sqlQuery.exec(sqlCmd);
                sendMessage(QString("userRegisterSuccess:%1").arg(ID));
            }
        }
        else if(cmd.contains("publishInfo"))
        {
            //
            QString tempString=dataStream.readAll();
            QStringList tempList = tempString.split("`");
            QString ID,username,fromplace,toplace,date,detail,time;
            ID=tempList[0];
            ID.replace(" ","");
            username=tempList[1];
            fromplace=tempList[2];
            toplace=tempList[3];
            date=tempList[4];
            detail=tempList[5];
            time=tempList[6];
            //dataStream >> ID >> username >> fromplace >> toplace >> date >> detail >> time;
            sqlCmd=QString("SELECT * FROM GroupList");
            sqlQuery.exec(sqlCmd);
            sqlQuery.last();
            int No = sqlQuery.at();
            if(No>=0)
            {
                No+=2;
            }
            else
            {
                No=1;
            }
            QString GroupID;
            GroupID.sprintf("%09d",No);

            sqlCmd=QString("INSERT INTO Info (GroupID,ID,username,fromplace,toplace,date,detail,time) VALUES ('%1','%2','%3','%4','%5','%6','%7','%8')").arg(GroupID).arg(ID).arg(username).arg(fromplace).arg(toplace).arg(date).arg(detail).arg(time);
            sqlQuery.exec(sqlCmd);
            //
            sqlCmd=QString("INSERT INTO GroupList (GroupID,CreatorID,CreatorUsername,fromplace,toplace,date,detail,MemberCount,MemberList) VALUES ('%1','%2','%3','%4','%5','%6','%7','%8','%9')").arg(GroupID).arg(ID).arg(username).arg(fromplace).arg(toplace).arg(date).arg(detail).arg(1).arg(username+"|");
            sqlQuery.exec(sqlCmd);

            sqlCmd=QString("Create Table GroupMessage%1(MsgNo varchar(10),Sender varchar(20),Date varchar(10),Message varchar(100),UnsendList varchar(50))").arg(GroupID);
            sqlQuery.exec(sqlCmd);

            sendMessage(QString("publishInfoSuccess:%1").arg(GroupID));
        }
        else if(cmd.contains("UpdateHeadImageVersion"))
        {
            QString ID,HeadImageVersion;
            dataStream >> ID >> HeadImageVersion;
            sqlCmd=QString("UPDATE user SET HeadImageVersion=%1 WHERE ID='%2'").arg(HeadImageVersion).arg(ID);
            sqlQuery.exec(sqlCmd);
            sendMessage("uploadSuccess");
        }
        else if(cmd.contains("InfoListRefresh"))
        {
            sqlCmd=QString("SELECT * FROM Info");
            sqlQuery.exec(sqlCmd);
            qint8 infoCount=sqlQuery.size();
            if(infoCount<=0)
            {
                sendMessage("InfoRefreshNone");
                return;
            }
            //if(infoCount>10) infoCount=10;
            QString InfoRefreshAnswer;
            InfoRefreshAnswer.append(QString("%1`").arg(infoCount));
            for(int i=0;i<infoCount;i++)
            {
                sqlCmd=QString("SELECT * FROM Info ORDER BY Time Desc");
                sqlQuery.exec(sqlCmd);
                sqlQuery.seek(i);
                QString Index,ID,HeadImageVersion,Username,From,To,Date,Detail,Time,MemberCount,MemberList;
                Index=sqlQuery.value("GroupID").toString();
                ID=sqlQuery.value("ID").toString();
                Username=sqlQuery.value("username").toString();
                From=sqlQuery.value("fromplace").toString();
                To=sqlQuery.value("toplace").toString();
                Date=sqlQuery.value("date").toString();
                Detail=sqlQuery.value("detail").toString();
                Time=sqlQuery.value("time").toString();
                sqlCmd=QString("SELECT * FROM GroupList WHERE GroupID='%1'").arg(Index);
                sqlQuery.exec(sqlCmd);
                sqlQuery.last();
                MemberCount=sqlQuery.value("MemberCount").toString();
                MemberList=sqlQuery.value("MemberList").toString();
                sqlCmd=QString("SELECT * FROM user WHERE ID='%1'").arg(ID);
                sqlQuery.exec(sqlCmd);
                sqlQuery.last();
                HeadImageVersion=sqlQuery.value("HeadImageVersion").toString();
                InfoRefreshAnswer.append(Index+"`");
                InfoRefreshAnswer.append(ID+"`");
                InfoRefreshAnswer.append(HeadImageVersion+"`");
                InfoRefreshAnswer.append(Username+"`");
                InfoRefreshAnswer.append(From+"`");
                InfoRefreshAnswer.append(To+"`");
                InfoRefreshAnswer.append(Date+"`");
                InfoRefreshAnswer.append(Detail+"`");
                InfoRefreshAnswer.append(Time+"`");
                InfoRefreshAnswer.append(MemberCount+"`");
                InfoRefreshAnswer.append(MemberList+"`");
            }
            sendMessage(InfoRefreshAnswer);
        }
        else if(cmd.contains("DiscussListRefresh"))

        {
            QString ID,HeadImageVersion ;
            dataStream >> ID;
            sqlCmd=QString("SELECT * FROM user WHERE ID='%1'").arg(ID);
            sqlQuery.exec(sqlCmd);
            sqlQuery.last();
            HeadImageVersion=sqlQuery.value("HeadImageVersion").toString();
            sendMessage(HeadImageVersion);
        }
        else if(cmd.contains("JoinGroup"))
        {
            QString GroupID,username;
            int memberCount;
            dataStream >> GroupID >> username;
            sqlCmd=QString("SELECT * FROM GroupList WHERE GroupID='%1'").arg(GroupID);
            sqlQuery.exec(sqlCmd);
            sqlQuery.last();
            username=sqlQuery.value("MemberList").toString()+username+"|";
            memberCount=sqlQuery.value("MemberCount").toInt()+1;
            sqlCmd=QString("UPDATE GroupList SET MemberCount='%1',MemberList='%2' WHERE GroupID='%3'").arg(memberCount).arg(username).arg(GroupID);
            sqlQuery.exec(sqlCmd);
            sendMessage("JoinGroupSuccess");
        }
        else if(cmd.contains("DiscussRefresh"))
        {
            QString username;
            dataStream >> username;
            sqlCmd=QString("SELECT * FROM GroupList WHERE MemberList Like '\%%1\%'").arg(username);
            sqlQuery.exec(sqlCmd);
            qint8 infoCount=sqlQuery.size();
            if(infoCount<=0)
            {
                sendMessage("DiscussRefreshNone");
                return;
            }

            QString DiscussRefreshAnswer;
            DiscussRefreshAnswer.append(QString("%1`").arg(infoCount));

            for(int i=0;i<infoCount;i++)
            {
                sqlCmd=QString("SELECT * FROM GroupList WHERE MemberList Like '\%%1\%'").arg(username);
                sqlQuery.exec(sqlCmd);
                sqlQuery.seek(i);
                QString GroupID,ID,HeadImageVersion,Username,From,To,Date,Detail,MemberCount,MemberList;
                GroupID=sqlQuery.value("GroupID").toString();
                ID=sqlQuery.value("CreatorID").toString();
                Username=sqlQuery.value("CreatorUsername").toString();
                From=sqlQuery.value("fromplace").toString();
                To=sqlQuery.value("toplace").toString();
                Date=sqlQuery.value("date").toString();
                Detail=sqlQuery.value("detail").toString();
                MemberCount=sqlQuery.value("MemberCount").toString();
                MemberList=sqlQuery.value("MemberList").toString();
                sqlCmd=QString("SELECT * FROM user WHERE ID='%1'").arg(ID);
                sqlQuery.exec(sqlCmd);
                sqlQuery.last();
                HeadImageVersion=sqlQuery.value("HeadImageVersion").toString();
                DiscussRefreshAnswer.append(GroupID+"`");
                DiscussRefreshAnswer.append(ID+"`");
                DiscussRefreshAnswer.append(HeadImageVersion+"`");
                DiscussRefreshAnswer.append(Username+"`");
                DiscussRefreshAnswer.append(From+"`");
                DiscussRefreshAnswer.append(To+"`");
                DiscussRefreshAnswer.append(Date+"`");
                DiscussRefreshAnswer.append(Detail+"`");
                DiscussRefreshAnswer.append(MemberCount+"`");
                DiscussRefreshAnswer.append(MemberList+"`");
            }
            sendMessage(DiscussRefreshAnswer);
        }/*
        else if(cmd.contains("GroupInfoRefresh"))
        {
            QString username;
            int memberCount;
            dataStream >> memberCount;
            QString GroupInfoRefreshAnswer;
            GroupInfoRefreshAnswer.append(QString("%1 ").arg(memberCount));
            for(int i=0;i<memberCount;i++)
            {
                dataStream >> username;
                sqlCmd=QString("SELECT * FROM user WHERE username='%1'").arg(username);
                sqlQuery.exec(sqlCmd);
                sqlQuery.last();
                QString ID,HeadImageVersion,Username,Sex,Telephone;
                ID=sqlQuery.value("ID").toString();
                Username=sqlQuery.value("username").toString();
                Sex=sqlQuery.value("sex").toString();
                Telephone=sqlQuery.value("telephone").toString();
                HeadImageVersion=sqlQuery.value("HeadImageVersion").toString();
                GroupInfoRefreshAnswer.append(ID+" ");
                GroupInfoRefreshAnswer.append(HeadImageVersion+" ");
                GroupInfoRefreshAnswer.append(Username+" ");
                GroupInfoRefreshAnswer.append(Sex+" ");
                GroupInfoRefreshAnswer.append(Telephone+" ");
            }
            sendMessage(GroupInfoRefreshAnswer);
            mainWindow->SetLineText(GroupInfoRefreshAnswer);
        }*/
        else if(cmd.contains("GroupInfoRefresh2"))
        {
            QString GroupID;
            dataStream >> GroupID;
            sqlCmd=QString("SELECT * FROM GroupList WHERE GroupID='%1'").arg(GroupID);
            sqlQuery.exec(sqlCmd);
            sqlQuery.last();
            QString memberList=sqlQuery.value("MemberList").toString();
            int memberCount=sqlQuery.value("MemberCount").toInt();
            QStringList member=memberList.split("|");
            QString GroupInfoRefreshAnswer;
            GroupInfoRefreshAnswer.append(QString("%1 ").arg(memberCount));
            for(int i=0;i<memberCount;i++)
            {
                sqlCmd=QString("SELECT * FROM user WHERE username='%1'").arg(member[i]);
                sqlQuery.exec(sqlCmd);
                sqlQuery.last();
                QString ID,HeadImageVersion,Username,Sex,Telephone;
                ID=sqlQuery.value("ID").toString();
                Username=sqlQuery.value("username").toString();
                Sex=sqlQuery.value("sex").toString();
                Telephone=sqlQuery.value("telephone").toString();
                HeadImageVersion=sqlQuery.value("HeadImageVersion").toString();
                GroupInfoRefreshAnswer.append(ID+" ");
                GroupInfoRefreshAnswer.append(HeadImageVersion+" ");
                GroupInfoRefreshAnswer.append(Username+" ");
                GroupInfoRefreshAnswer.append(Sex+" ");
                GroupInfoRefreshAnswer.append(Telephone+" ");
            }
            sendMessage(GroupInfoRefreshAnswer);
        }
        else if(cmd.contains("DischargeGroup"))
        {
            QString GroupID;
            dataStream >> GroupID;
            sqlCmd=QString("UPDATE GroupList SET MemberCount='0',MemberList='Empty' WHERE GroupID='%1'").arg(GroupID);
            sqlQuery.exec(sqlCmd);
        }
        else if(cmd.contains("ExitGroup"))
        {
            QString GroupID,username;
            dataStream >> GroupID >> username;
            username+="|";
            sqlCmd=QString("SELECT * FROM GroupList WHERE GroupID='%1'").arg(GroupID);
            sqlQuery.exec(sqlCmd);
            sqlQuery.last();
            QString memberList=sqlQuery.value("MemberList").toString();
            int memberCount=sqlQuery.value("MemberCount").toInt()-1;
            memberList.replace(username,"");
            sqlCmd=QString("UPDATE GroupList SET MemberCount='%1',MemberList='%2' WHERE GroupID='%3'").arg(memberCount).arg(memberList).arg(GroupID);
            sqlQuery.exec(sqlCmd);
        }
        else if(cmd.contains("DeleteMember"))
        {
            QString GroupID,username;
            dataStream >> GroupID >> username;
            username+="|";
            sqlCmd=QString("SELECT * FROM GroupList WHERE GroupID='%1'").arg(GroupID);
            sqlQuery.exec(sqlCmd);
            sqlQuery.last();
            QString memberList=sqlQuery.value("MemberList").toString();
            int memberCount=sqlQuery.value("MemberCount").toInt()-1;
            memberList.replace(username,"");
            sqlCmd=QString("UPDATE GroupList SET MemberCount='%1',MemberList='%2' WHERE GroupID='%3'").arg(memberCount).arg(memberList).arg(GroupID);
            sqlQuery.exec(sqlCmd);
        }
        else if(cmd.contains("SetTelephone"))
        {
            QString ID,telephone;
            dataStream >> ID >> telephone;
            sqlCmd=QString("UPDATE user SET telephone='%1' WHERE ID='%2'").arg(telephone).arg(ID);
            sqlQuery.exec(sqlCmd);
            sendMessage("SetTelephoneSuccess");
        }
        else if(cmd.contains("SetPassword"))
        {
            QString ID,password;
            dataStream >> ID >> password;
            sqlCmd=QString("UPDATE user SET password='%1' WHERE ID='%2'").arg(password).arg(ID);
            sqlQuery.exec(sqlCmd);
            sendMessage("SetPasswordSuccess");
        }
        else if(cmd.contains("NearByRefresh"))
        {
            QString ID,Longitude,Latitude,Destination;
            dataStream >> ID >> Longitude >> Latitude >> Destination;
            sqlCmd=QString("UPDATE OnlineUser SET Longitude='%1',Latitude='%2',Destination='%3' WHERE ID='%4'").arg(Longitude).arg(Latitude).arg(Destination).arg(ID);
            sqlQuery.exec(sqlCmd);
            sqlCmd=QString("SELECT * FROM OnlineUser WHERE ID!='%1' and Longitude!='-' and Latitude!='-'").arg(ID);
            sqlQuery.exec(sqlCmd);
            qint8 sqlCount=sqlQuery.size();
            if(sqlCount>0)
            {
                QString sendMsg;
                sendMsg=QString("%1`").arg(sqlCount);
                for(int i=0;i<sqlCount;i++)
                {
                    sqlQuery.seek(i);
                    QString userID,HeadImageVersion,username,telnum,longitude,latitude,destination;
                    longitude=sqlQuery.value("Longitude").toString();
                    latitude=sqlQuery.value("Latitude").toString();
                    double LonA,LonB,LatA,LatB,C,distance;
                    LonA=Longitude.toDouble();
                    LonB=longitude.toDouble();
                    LatA=Latitude.toDouble();
                    LatB=latitude.toDouble();
                    C=sin(LatA/57.2958)*sin(LatB/57.2958)+cos(LatA/57.2958)*cos(LatB/57.2958)*cos((LonA-LonB)/57.2958);
                    distance=6371.004*acos(C)*1000;
                    if(distance<10000)
                    {
                        if(distance<=10) distance=10;
                        else if(distance<=50) distance=50;
                        else if(distance<=100) distance=100;
                        else if(distance<=200) distance=200;
                        else if(distance<=500) distance=500;
                        else distance=1000;

                        userID=sqlQuery.value("ID").toString();
                        username=sqlQuery.value("username").toString();
                        destination=sqlQuery.value("Destination").toString();
                        QSqlQuery tempQuery(dataBase->database());
                        tempQuery.exec(QString("SELECT * FROM user WHERE ID='%1'").arg(userID));
                        tempQuery.seek(0);
                        HeadImageVersion=tempQuery.value("HeadImageVersion").toString();
                        telnum=tempQuery.value("telephone").toString();
                        sendMsg.append(userID+"`");
                        sendMsg.append(HeadImageVersion+"`");
                        sendMsg.append(username+"`");
                        sendMsg.append(telnum+"`");
                        sendMsg.append(QString("%1").arg(distance)+"`");
                        sendMsg.append(destination+"`");
                    }
                }
                sendMessage(sendMsg);
            }
            else
            {
                sendMessage("NearByRefreshNone");
            }
        }
        else if(cmd.contains("SetAdvice"))
        {
            QString tempString;
            dataStream>>tempString;
            QStringList tempList=tempString.split("`");
            QString ID,username,advice;
            ID=tempList[0];
            username=tempList[1];
            advice=tempList[2];
            sqlCmd=QString("SELECT * FROM Advice");
            sqlQuery.exec(sqlCmd);
            int count=sqlQuery.size();
            if(count<=0) count=0;
            count++;
            sqlCmd=QString("INSERT INTO Advice (No,UserID,Username,Advice) VALUES('%1','%2','%3','%4')").arg(count).arg(ID).arg(username).arg(advice);
            sqlQuery.exec(sqlCmd);
            sendMessage("SetAdviceSuccess");
        }
        else if(cmd.contains("VersionCheck"))
        {
            sendMessage("Version 1.0");
        }
    }
    //dataBase->close();
}

void myThread::sendMessage(QString sendString)

{
    QByteArray block;
    block.append(sendString);
    mTcpSocket->write(block);
}

