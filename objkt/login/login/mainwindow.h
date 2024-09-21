#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QDebug>
#include <QTcpSocket>
#include <QMouseEvent>
#include <QRect>
#include <QApplication>
#include <QScreen>
#include <QMessageBox>
#include "common.h"
#include "form.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void on_pb_login_clicked();

    void on_exit_clicked();

    void on_pb_register_clicked();

    void onReadyRead();

    void showThisWindow();
private:
    Ui::MainWindow *ui;
    bool dragging;
    QPoint dragPosition;
    QPoint dragOrigin;
    QTcpSocket *clientSocket;
    Form *newForm;
};
#endif // MAINWINDOW_H
