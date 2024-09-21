#include "sql.h"

My_Sql::My_Sql()
{
    if(QSqlDatabase::contains("qt_sql_default_connection") == true)
    {
        database = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        database = QSqlDatabase::addDatabase("QSQLITE");
        database.setDatabaseName("MyDataBase.db");
    }
}

bool My_Sql::open_db()
{
    if(false == database.open())
    {
        qDebug()<<"Error::Failed to connet database"<<database.lastError();
    }
    return true;
}

void My_Sql::close_db()
{
    database.close();
}

void My_Sql::normalExec(QString sql)
{
    QSqlQuery sql_Query;
    if(!sql_Query.exec(sql))
    {
        qDebug()<<sql_Query.lastError();
    }
    else
    {
        qDebug()<<"数据库创建成功";
    }
}

void My_Sql::normalQuery(QSqlQuery &sql_Query, QString sql)
{
    if(!sql_Query.exec(sql))
        {
            qDebug()<<sql_Query.lastError();
        }
        else
        {
            qDebug()<<"数据库操作执行成功";
    }
}

void My_Sql::importCitiesFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error opening file:" << file.errorString();
        return;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");
    QSqlQuery query(database);
    int i = 1;
    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            QStringList fields = line.split("\t");
            if (fields.size() == 2) {
                bool ok;
                int id = fields[0].toInt(&ok);
                if (ok) {
                    QString name = fields[1];
                    // 构造INSERT语句
                    QString sql = QString("insert into user_city values('%1','%2')").arg(id).arg(name.replace("'", "''")); // 替换单引号以避免SQL注入

                    // 执行INSERT语句
                    if (!query.exec(sql)) {
                        qDebug() << "Error inserting data:" << query.lastError().text();
                    }
                }
            }
        }
        qDebug() << i++;
    }
    file.close();
}

void My_Sql::readFirstRowFromDatabase()
{
    QSqlQuery query(database);

        // 执行SQL查询，这里以选择cities表中的所有列为例
        if (!query.exec("SELECT * FROM user_city")) {
            qDebug() << "Error executing query:" << query.lastError().text();
            return;
        }
        if (query.next()) {
            QString citykey = query.value("citykey").toString(); // 假设id是整数类型
            QString city = query.value("city").toString(); // 假设name是字符串类型

            qDebug() << "citykey:" << citykey << "city:" << city;
        } else {
            // 如果没有找到任何行，则输出相应信息
            qDebug() << "No rows found in the table.";
        }
}

