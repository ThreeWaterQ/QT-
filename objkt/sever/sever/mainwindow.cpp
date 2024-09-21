#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 创建服务器对象
    server = new QTcpServer(this);
     // 设置端口号
    server->listen(QHostAddress::Any, 9000);
    // 连接信号
    connect(server, &QTcpServer::newConnection, this, &MainWindow::acceptConnect);
    my_sql = new My_Sql;
    my_sql->open_db();
//    my_sql->normalExec("DELETE FROM user_city;");
//    my_sql->normalExec("UPDATE user_city  SET city = '黄山'  WHERE citykey = '101221008';");
//    my_sql->importCitiesFromFile(":/city.txt");
//    my_sql->importCitiesFromFile(":/city_2.txt");

}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::acceptConnect()
{
    //返回连接的套接字对象
   clientSocket = server->nextPendingConnection();
   connect(clientSocket, &QTcpSocket::readyRead, this, &MainWindow::onReadyRead);
   connect(clientSocket, &QTcpSocket::disconnected, this, &MainWindow::onDisconnected);
}
 // 槽函数
void MainWindow::onReadyRead() {
     if (clientSocket) {
         MSG_DT read_msg;
         int len = clientSocket->read((char *)&read_msg,sizeof(read_msg));
         register_node *read_node = (register_node*)read_msg.buf;
         ui->textEdit->append(read_node->name);
         ui->textEdit->append(read_node->password);
         QString postback;
         if(read_msg.type == 1)
         {
             QSqlQuery sqlQuery;//这个结构体用来存放查询的结果
             my_sql->normalQuery(sqlQuery, QString("select * from user where name='%1'").arg(read_node->name));
             if(sqlQuery.next())
             {
                 postback = "用户名已存在";
                 qDebug()<<postback;
             }
             else
             {
                 QString sql = QString("insert into user values('%1','%2')").arg(read_node->name).arg(read_node->password);
                 my_sql->normalExec(sql);
                 postback = "注册成功";
                 qDebug()<<postback;
             }
         }
         else if(read_msg.type == 2)
         {
             QSqlQuery sqlQuery;//这个结构体用来存放查询的结果
             my_sql->normalQuery(sqlQuery, QString
                                 ("select * from user where name='%1' and password='%2'")
                                 .arg(read_node->name).arg(read_node->password));
             if(sqlQuery.next())
             {
                 postback = "登陆成功";
                 qDebug()<<postback;
             }
             else
             {
                 postback = "登陆失败，用户名或密码错误";
                 qDebug()<<postback;
             }

         }
         QByteArray postbackBytes = postback.toUtf8();
         clientSocket->write(postbackBytes);
     }
}
void MainWindow::onDisconnected() {
     if (clientSocket) {
        qDebug()<<"客户端退出了"<<endl;
     }
}
