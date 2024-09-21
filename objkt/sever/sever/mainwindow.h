#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include "common.h"
#include "sql.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void onReadyRead();
    void onDisconnected();
    void acceptConnect();

private:
    Ui::MainWindow *ui;
    QTcpServer *server;
    QTcpSocket *clientSocket;
    My_Sql *my_sql;
};
#endif // MAINWINDOW_H
