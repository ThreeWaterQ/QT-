#include "mainwindow.h"
#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    newForm = new Form();
    connect(newForm,SIGNAL(showMainWindow()),this,SLOT(showThisWindow()));
    QPixmap beijing_picture_name(":/bj.jpg");
    ui->beijing->setPixmap(beijing_picture_name);
    QPixmap head_picture_name(":/head_picture.png");
    ui->head_pt->setPixmap(head_picture_name);
    ui->exit->setStyleSheet("border-image: url(:/cha.jfif); border: 2px solid black;");

    this->setWindowFlag(Qt::FramelessWindowHint);//删除边框

    clientSocket = new QTcpSocket(this);
    // 尝试连接到服务器
    clientSocket->connectToHost("172.28.29.76", 9000);
    connect(clientSocket, &QTcpSocket::readyRead, this, &MainWindow::onReadyRead);
}

MainWindow::~MainWindow()//析构，关闭
{
    delete ui;
}
//--------------------------------------------------------------------
// 在 mousePressEvent 中开始拖动
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragging = true;
        dragOrigin = frameGeometry().topLeft(); // 获取窗口的左上角位置
        dragPosition = event->globalPos(); // 记录鼠标按下时的全局位置
    }
    QMainWindow::mousePressEvent(event);
}

// 在 mouseReleaseEvent 中停止拖动
void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragging = false;
    }
    QMainWindow::mouseReleaseEvent(event);
}

// 处理鼠标移动事件以拖动窗口
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (dragging) {
        // 计算新的窗口位置
        QPoint newPos = dragOrigin + (event->globalPos() - dragPosition);

        // 获取当前屏幕的几何信息
        QScreen *screen = QGuiApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();

        // 限制新位置在屏幕范围内
        newPos.setX(qMax(screenGeometry.left(), qMin(newPos.x(), screenGeometry.right() - width())));
        newPos.setY(qMax(screenGeometry.top(), qMin(newPos.y(), screenGeometry.bottom() - height())));

        // 移动窗口
        move(newPos);
    }
    QMainWindow::mouseMoveEvent(event);
}

//------------------------------------------------------------
void MainWindow::on_pb_login_clicked()//登录
{
    MSG_DT msg;
    msg.type = 2;//登录
    QString user_name_str = ui->user_name->text();
    QString user_password_str = ui->password->text();
    if(user_name_str.isEmpty() == 1 || user_password_str.isEmpty() == 1)
    {
        QMessageBox msgBox;
        msgBox.setText("用户名或密码不能为空");
        msgBox.exec();
        return;
    }
    std::string str_name = user_name_str.toStdString();
    std::string str_password = user_password_str.toStdString();
    const char * p_name = str_name.c_str();
    const char * p_password = str_password.c_str();
    register_node reg(p_name,p_password);
    memcpy(msg.buf,&reg,sizeof(reg));
    clientSocket->write((char*)&msg,sizeof(msg));//建立链接成功，发送
}

void MainWindow::on_exit_clicked()
{
    close();
}


void MainWindow::on_pb_register_clicked()//注册
{
    MSG_DT msg;
    msg.type = 1;//注册
    QString user_name_str = ui->user_name->text();
    QString user_password_str = ui->password->text();
    if(user_name_str.isEmpty() == 1 || user_password_str.isEmpty() == 1)
    {
        QMessageBox msgBox;
        msgBox.setText("用户名或密码不能为空");
        msgBox.exec();
        return;
    }
    std::string str_name = user_name_str.toStdString();
    std::string str_password = user_password_str.toStdString();
    const char * p_name = str_name.c_str();
    const char * p_password = str_password.c_str();
    register_node reg(p_name,p_password);
    memcpy(msg.buf,&reg,sizeof(reg));
    clientSocket->write((char*)&msg,sizeof(msg));//建立链接成功，发送
}

void MainWindow::onReadyRead()//获得服务器返回信息,登录跳转
{
    if (clientSocket)
    {
        QByteArray read = clientSocket->readAll();
        QString QString_read = QString::fromUtf8(read);
        QMessageBox msgBox;
        msgBox.setText(QString_read);
        msgBox.exec();
        newForm->show();
        this->hide();
        if(QString_read == "登陆成功")
        {
            newForm->show();
            this->hide();
        }
    }
}

void MainWindow::showThisWindow()//显示登录窗口
{
    this->show();
}
