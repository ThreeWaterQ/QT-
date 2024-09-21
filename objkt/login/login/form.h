#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QPainter>
#include <QString>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonParseError>
#include <QTime>
#include <QTimer>
#include <QMainWindow>
#include <QDebug>
#include <QMouseEvent>
#include <QRect>
#include <QApplication>
#include <QScreen>
#include <QMessageBox>
#include <QCompleter>
#include <QStringListModel>
#include <QCoreApplication>
#include <QRegularExpression>
#include "sql.h"

namespace Ui {
class Form;
}
struct day_data
{
    QString week;
    QString ymd;
    QString wendu;
    QString type;
    QString wind;
    int aqi;
};

struct weather_data
{
    QString city;
    QString time;
    QString wendu;
    QString notice;
    day_data forecast[5];
};

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = nullptr);
    void parseJson(QByteArray array);
    void set_ui();
    void timerUpdate();
    void initiateRequest();
    ~Form();
public slots:
    void processWeatherReply(QNetworkReply *reply);
    void processInfoReply(QNetworkReply *reply);

signals:
    void showMainWindow();
private slots:
    void on_pb_back_clicked();

    void on_pb_exit_clicked();

    void on_pb_sousuo_clicked();

    QStringList queryCities(const QString &prefix);

    void handleNetworkReply(QNetworkReply *reply);
private:
    Ui::Form *ui;
    My_Sql my_sql;
    weather_data *now;
    bool dragging;
    QPoint dragPosition;
    QPoint dragOrigin;
    QTcpSocket *clientSocket;
    QNetworkAccessManager *manager;
protected:
    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // FORM_H
