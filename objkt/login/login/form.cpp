#include "form.h"
#include "ui_form.h"
Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form),
    manager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::FramelessWindowHint);//删除边框
    my_sql.open_db();
    now = new weather_data();
    QTimer *timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, &Form::timerUpdate);
    timer->start(1000);

    // 创建QCompleter
    QCompleter *completer = new QCompleter(this);
    // 设置完成器模型为QStringListModel，初始为空
    QStringListModel *model = new QStringListModel(this);
    completer->setModel(model);
    // 连接到QLineEdit的textEdited信号
    connect(ui->city_txt, &QLineEdit::textEdited, this, [this, completer, model](const QString &text)
    {
        QStringList cities = queryCities(text);
        QCompleter *completer = new QCompleter(cities, this);
        ui->city_txt->setCompleter(completer);
        //这里每次文本变化都创建新的QCompleter
    });
    // 将完成器设置给QLineEdit
    ui->city_txt->setCompleter(completer);

    //------------------------------------
     connect(manager, &QNetworkAccessManager::finished, this, &Form::handleNetworkReply);
    //初始化本地天气
    // 连接信号到槽以处理响应
    QUrl url(QString("http://ip.useragentinfo.com/json?ip="));
    QNetworkRequest request(url);
    manager->get(request);
}

void Form::parseJson(QByteArray array)//josn转字符串存入结构体
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(array,&error);
    if(error.error !=QJsonParseError::NoError)
    {
        qDebug("josn error");
        return ;
    }

    QJsonObject obj = doc.object();
    //判断有无城市数据
    int status = obj.value("status").toInt();
    if(status == 403)
    {
        QMessageBox msgBox;
        msgBox.setText("暂无此城市数据");
        msgBox.exec();
        return;
    }
    //时间
    QString time = obj.value("time").toString();
    now->time = time;//now没有初始化，没有给空间,程序会异常结束
    qDebug()<<now->time<<"时间";
    //地区
    QJsonObject cityobj = obj.value("cityInfo").toObject();
    QString city = cityobj.value("city").toString();
    now->city = city;
    qDebug()<<now->city<<"地区";
    QJsonObject dataobj = obj.value("data").toObject();
    //实时温度
    QString wendu = dataobj.value("wendu").toString();
    wendu +="℃";
    now->wendu = wendu;
    qDebug()<<now->wendu<<"实时温度";
    QJsonArray notice_arr = dataobj.value("forecast").toArray();
    QJsonObject notice_obj = notice_arr.at(0).toObject();
    QString notice = notice_obj.value("notice").toString();
    now->notice = notice;

    //近一周的天气
    QJsonArray forecast_array = dataobj.value("forecast").toArray();
    for(int i =0;i<5;i++)
    {
        QJsonObject weatherobj = forecast_array.at(i).toObject();
        //最高温度
        QString high = weatherobj.value("high").toString();
        high = high.mid(3);
        //最低温度
        QString low = weatherobj.value("low").toString();
        low = low.mid(3,2);
        //温度范围
        QString range_wendu = low+"~"+high;
        now->forecast[i].wendu = range_wendu;
        qDebug()<<now->forecast[i].wendu<<"温度范围";

        //日期
        QString ymd = weatherobj.value("ymd").toString();
        ymd = ymd.mid(5);
        now->forecast[i].ymd = ymd;
        qDebug()<<now->forecast[i].ymd<<"日期";
        //星期几
        now->forecast[i].week =weatherobj.value("week").toString();
        qDebug()<<now->forecast[i].week<<"星期几";

        //天气
        QString type = weatherobj.value("type").toString();
        now->forecast[i].type = type;
        qDebug()<<now->forecast[i].type<<"天气";

        //风
        QString fx = weatherobj.value("fx").toString();
        QString fl = weatherobj.value("fl").toString();
        QString wind = fx+fl;
        now->forecast[i].wind = wind;
        qDebug()<<now->forecast[i].wind<<"风";

        //空气质量
        int aqi = weatherobj.value("aqi").toInt();
        now->forecast[i].aqi = aqi;
        qDebug()<<now->forecast[i].aqi<<"空气质量"<<endl;
    }
    set_ui();
}

void Form::set_ui()//更新图片及字体
{
    QVector<QLabel*> weeks = {ui->week_1, ui->week_2, ui->week_3, ui->week_4, ui->week_5};
    QVector<QLabel*> ymds = {ui->ymd_1, ui->ymd_2, ui->ymd_3, ui->ymd_4, ui->ymd_5};
    QVector<QLabel*> wendus = {ui->wendu_1, ui->wendu_2, ui->wendu_3, ui->wendu_4, ui->wendu_5};
    QVector<QLabel*> types = {ui->pc_1, ui->pc_2, ui->pc_3, ui->pc_4, ui->pc_5};
    QVector<QLabel*> types_p = {ui->type_1, ui->type_2, ui->type_3, ui->type_4, ui->type_5};
    QVector<QLabel*> winds = {ui->wind_1, ui->wind_2, ui->wind_3, ui->wind_4, ui->wind_5};


    for(int i = 0; i < weeks.size(); ++i) {
        weeks[i]->setText(now->forecast[i].week);
        wendus[i]->setText(now->forecast[i].wendu);
        types_p[i]->setText(now->forecast[i].type);
        ymds[i]->setText(now->forecast[i].ymd);
        winds[i]->setText(now->forecast[i].wind);
        QString pixmap_tou = ":/xiaotubiao/";
        pixmap_tou += now->forecast[i].type;
        pixmap_tou += ".png";
        QPixmap pixmap(pixmap_tou);
        types[i]->setPixmap(pixmap);
    }
    QPalette pal =ui->widget_bj->palette();
    QString pc_bj_math;
    if(now->forecast[0].type=="暴雨"||now->forecast[0].type=="大暴雨"||
            now->forecast[0].type=="大雨"||now->forecast[0].type=="冻雨"||
            now->forecast[0].type=="雷阵雨"||now->forecast[0].type=="特大暴雨"||
            now->forecast[0].type=="小雨"||now->forecast[0].type=="中雨")
    {
        pc_bj_math = ":/bj_雨.jpg";
    }
    else if(now->forecast[0].type=="暴雪"||now->forecast[0].type=="冰雹"||
            now->forecast[0].type=="大雪"||now->forecast[0].type=="小雪"||
            now->forecast[0].type=="雨夹雪"||now->forecast[0].type=="中雪")
    {
        pc_bj_math = ":/bj_雪.jpg";
    }
    else if(now->forecast[0].type=="多云"||now->forecast[0].type=="雾霾"||
            now->forecast[0].type=="雾"||now->forecast[0].type=="阴")
    {
        pc_bj_math = ":/bj_云.jpg";
    }
    else if(now->forecast[0].type=="晴")
    {
        pc_bj_math = ":/bj_晴.jpg";
    }
    pal.setBrush(QPalette::Background,QBrush(QPixmap(pc_bj_math)));
    setPalette(pal);
    //ui->time_1->setText(now->time);//时间不是实时
    ui->city->setText(now->city);
    ui->wendu->setText(now->wendu);
    ui->notice->setText(now->notice);
}

void Form::timerUpdate()//按格式获得实时时间
{
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyy-MM-dd hh:mm:ss dddd");
    ui->time_1->setText(str);
}

Form::~Form()//析构
{
    delete ui;
}

void Form::processWeatherReply(QNetworkReply *reply)
{
    QVariant ret = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(ret == 200)//200说明页面访问成功
    {
        QByteArray data = reply->readAll();//读取所有的网络数据
        parseJson(data);
        qDebug()<<"访问成功"<<endl;
    }
    else
    {
         qDebug()<<"没访问成功"<<endl;
    }
}

void Form::processInfoReply(QNetworkReply *reply)
{
    QVariant ret = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    qDebug() << ret ;
    if(ret == 200)//200说明页面访问成功
    {
        QByteArray data = reply->readAll();//读取所有的网络数据
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(data,&error);
        if(error.error !=QJsonParseError::NoError)
        {
            qDebug("josn error");
            return ;
        }
        QJsonObject jsonObject = doc.object();
        QString city = jsonObject.value("city").toString().toUtf8();
        int indexOfCityEnd = city.lastIndexOf("市"); // 找到“市”的索引
        if (indexOfCityEnd != -1) { // 确保找到了“市”
            QString cityName = city.left(indexOfCityEnd); // 提取“市”之前的所有字符
            qDebug() << cityName; // 输出结果
            ui->city_txt->setText(cityName);
            on_pb_sousuo_clicked();
        }
        qDebug()<<"访问成功"<<endl;
    }
    else
    {
         qDebug()<<"没访问成功"<<endl;
    }
}

void Form::on_pb_back_clicked()//返回按钮
{
    this->hide();
    showMainWindow();
}

void Form::on_pb_exit_clicked()//关闭按钮
{
    close();
}

void Form::on_pb_sousuo_clicked()//输入字符串，数据库对比城市url传入访问josn
{
    QString url = "http://t.weather.sojson.com/api/weather/city/";
    QString citykey;
    QString txt = ui->city_txt->text();
    QSqlQuery sqlQuery;
    // 使用Qt的QSqlQuery来执行查询
    QString queryStr = "SELECT citykey FROM user_city WHERE city = '%1'";
    queryStr = queryStr.arg(txt);

    if (!sqlQuery.exec(queryStr)) {
        // 处理查询失败的情况
        qDebug() << "Query failed:" << sqlQuery.lastError().text();
        return;
    }

    // 查询成功，检查是否有结果返回
    if (sqlQuery.next()) {
        // 现在sqlQuery对象定位在第一条记录上
        citykey = sqlQuery.value(0).toString();
    } else {
        // 没有找到任何匹配的行
        qDebug() << "No city with that name found.";
    }
    url+=citykey;
    QString url_type = "?type=weather";
    url+=url_type;
    qDebug() << "url:" << url;
    manager->get(QNetworkRequest(QUrl(url)));
}

//--------------------------------------------------------------------
// 在 mousePressEvent 中开始拖动
void Form::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragging = true;
        dragOrigin = frameGeometry().topLeft(); // 获取窗口的左上角位置
        dragPosition = event->globalPos(); // 记录鼠标按下时的全局位置
    }
    QWidget::mousePressEvent(event);
}

// 在 mouseReleaseEvent 中停止拖动
void Form::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragging = false;
    }
    QWidget::mouseReleaseEvent(event);
}

// 处理鼠标移动事件以拖动窗口
void Form::mouseMoveEvent(QMouseEvent *event)
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
    QWidget::mouseMoveEvent(event);
}

//------------------------------------------------------------

QStringList Form::queryCities(const QString &prefix)//数据库实现联想接口
{
    QStringList cities;
    QSqlQuery sqlQuery;
    // 构造SQL查询语句，使用参数化查询来避免SQL注入
    QString queryStr = "SELECT city FROM user_city WHERE city LIKE :prefix";
    sqlQuery.prepare(queryStr);
    sqlQuery.bindValue(":prefix", "%" + prefix + "%");
    // 执行查询
    if (!sqlQuery.exec()) {
        qDebug() << "查询失败:" << sqlQuery.lastError().text();
        return cities;
    }
    // 遍历查询结果
    while (sqlQuery.next()) {
        QString city = sqlQuery.value(0).toString(); // 获取当前行的city字段值
        cities.append(city); // 将城市名称添加到列表中
    }
    return cities;
}

void Form::handleNetworkReply(QNetworkReply *reply)
{
    QUrl url = reply->url();
    // 使用 QUrl::query() 获取查询字符串
    QString queryString = url.query();
    qDebug()<<queryString;

    if (queryString == "type=weather") {
        // 处理天气相关的回复
        processWeatherReply(reply);
    }
    else if(queryString == "ip=")
        processInfoReply(reply);
    // 清理回复对象
    reply->deleteLater();
}
