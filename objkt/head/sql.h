#ifndef SQL_H
#define SQL_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QSqlQuery>
class My_Sql
{
public:
    My_Sql();
    bool open_db(void);
    void close_db(void);
    void normalExec(QString sql);
    void normalQuery(QSqlQuery &sql_Query,QString sql);
    void importCitiesFromFile(const QString &filePath);
    void readFirstRowFromDatabase();
private:
    QSqlDatabase database;
};
#endif // SQL_H
