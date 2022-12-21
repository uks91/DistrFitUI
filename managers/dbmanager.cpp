// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "dbmanager.h"
#include <QDateTime>
#include <QtSql/QSql>

#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QStringList>
//#include <QFile>


//QSqlDatabase DBManager::db        = QSqlDatabase::addDatabase("QSQLITE");
//QSqlQuery    DBManager::query     = QSqlQuery (db);

//!
//! \brief DBManager::DBManager
//!

DBManager::DBManager(/*QObject *parent*/) /*: QObject(parent)*/
    : db(QSqlDatabase::addDatabase("QSQLITE")),
      query(QSqlQuery(db))
{
    //isInit  = false;
    //init ();
//    db.addDatabase("QSQLITE");
//    DBManager::query     = QSqlQuery (db);
//    QFile file ("debug.txt");
//    file.open(QFile::WriteOnly);
//    QTextStream out (&file);
    db.setDatabaseName("rawdata.db");
//    db.setUserName("user");
//    db.setPassword("user");
//    db.setHostName("qwe");
    if (!db.open())
    {
//        qDebug () << "Unable to connect to database! " << db.lastError();
//        out << "Unable to connect to DB: " << db.lastError().text();
        return;
    }
    init ();
}

void DBManager::createTable(const QString &tableName, const QStringList &fields)
{
    QString string;
    string = "CREATE TABLE IF NOT EXISTS %1 (%2);";
    string = string.arg(tableName, fields.join(", "));

    if (!query.exec(string))
    {
//        qDebug () << query.lastError() << query.lastQuery();
    }
}

bool DBManager::deleteQuery()
{
    if (this->wheres.isEmpty() || tableName.isEmpty())
    {
        qCritical () << "Something is empty!";
        return false;
    }

    QString where = whereString();

//    if (wheres.size() > 0)
//        where = "WHERE " + whereString();

    QString str = QString("DELETE FROM `%1` %2").arg(tableName, where);
    //db.transaction();
    bool result = query.exec(str);
    //db.commit();
    if (!result)
        qCritical () << query.lastQuery() << query.lastError();

    clear ();
    return result;
}

void DBManager::init()
{
    //db = QSqlDatabase::addDatabase("QSQLITE");

    QStringList fields;
    //! tbl_sets
    fields << "id INTEGER ";
    fields << "name TEXT";
    fields << "comment TEXT";
    fields << "date TEXT";
    fields << "distributions TEXT";
    fields << "methods TEXT";

    createTable("tbl_sets", fields);

    fields.clear();

    //! tbl_properties
    fields << "id INTEGER ";
    fields << "id_set INTEGER";
    fields << "name TEXT";
    fields << "data TEXT";
    fields << "results TEXT";
    fields << "range TEXT";

    createTable("tbl_properties", fields);
}

bool DBManager::insertQuery()
{
    if (tableName.isEmpty() || fields.isEmpty() || fields.size() != values.size())
    {
        qCritical () << "Something is wrong...";
        return false;
    }

    QString str = QString("INSERT INTO %1 (%2) VALUES (%3)").arg(tableName, fields.join(", "), values.join(", "));
    //db.transaction();
    bool result = query.exec(str);
    //db.commit();
    if (!result)
        qCritical () << query.lastQuery() << query.lastError();

    clear ();
    return result;
}

int DBManager::lastId(const QString &tableName)
{
    int id = 0;
    //VarTable tbl = select("SELECT `id` FROM `tbl_properties`");
    //qDebug () << QString("SELECT `id` FROM `%1`").arg(tableName);
    if (!query.exec(QString("SELECT `id` FROM `%1`").arg(tableName)))
    {
        qDebug () << "Error in MainWindow::lastId() : " << query.lastError() << query.lastQuery();
        return 0;
    }
    while (query.next())
    {
        int tmp = query.value(0).toInt();
        if (tmp > id)
            id = tmp;
    }
    id++;
    //qDebug () << "last id in" << tableName << "is" << id;
    return id;
}


void DBManager::prepare(const QString &fieldName, int value)
{
    prepare(fieldName, QString::number(value));
}

void DBManager::prepare(const QString &fieldName, double value)
{
    prepare(fieldName, QString::number(value));
}

void DBManager::prepare(const QString &fieldName, const QString &value)
{
    if (fields.contains(fieldName) || value.isEmpty() || fieldName.isEmpty())
        return ;

    fields << fieldName;
    values << QString("'%1'").arg(value);
}

void DBManager::prepareFields(const QString &fieldName)
{
    //! For Select
    fields << fieldName;
}

void DBManager::prepareTableName(const QString &tableName)
{
    DBManager::tableName = tableName;
}

void DBManager::prepareTables(const QString &tableName)
{
    //! For Select
    tableNames << tableName;
}

void DBManager::prepareWhere(const QString &fieldName, int value)
{
    prepareWhere(fieldName, QString().number(value));
}

void DBManager::prepareWhere(const QString &fieldName, double value)
{
    prepareWhere(fieldName, QString().number(value));
}

void DBManager::prepareWhere(const QString &fieldName, const QString &value)
{
    //! For Select, Delete

    if (value.isEmpty() || fieldName.isEmpty())
        return;

    wheres << qMakePair (fieldName, value);
}

bool DBManager::selectQuery(VarTable &tbl)
{
    if (fields.isEmpty() || tableNames.isEmpty() /*|| wheres.isEmpty()*/)
    {
        qCritical () << "Fields or tableNames are empty!" << fields.isEmpty() << tableNames.isEmpty();
        return false;
    }

    QString str = QString("SELECT %1 FROM %2 %3").arg(fields.join(", "), tableNames.join(", "), whereString());
    bool result = query.exec(str);
    clear ();
//    db.commit();
    if (!result)
    {
        qCritical () << query.lastQuery() << query.lastError();
        return result;
    }
    tbl.clear();

    QSqlRecord record = query.record();

    while (query.next())
    {
        QMap<QString,QVariant> map;
        for (int i=0; i<record.count(); i++)
            map[record.fieldName(i)] = query.value(i);
        tbl << map;
    }

    return result;
}

bool DBManager::updateQuery()
{
    QStringList sets;

    if (fields.size() == 0 || fields.size() != values.size() || this->wheres.size() == 0 || tableName.isEmpty())
    {
        qCritical () << "Something is wrong...";
        return false;
    }

    for (int i=0; i<fields.size(); i++)
        sets << QString("%1=%2").arg(fields.at(i), values.at(i));


    QString str = QString("UPDATE `%1` SET %2 %3;").arg(tableName, sets.join(", "), whereString());
    //db.transaction();
    bool result = query.exec(str);
    //db.commit();
    if (!result)
        qCritical () << query.lastQuery() << query.lastError();

    clear ();
    return result;
}

QString DBManager::lastError() const
{
    return db.lastError().text();
}

DBManager &DBManager::instance()
{
    static DBManager singleton;
    return singleton;
}

void DBManager::clear()
{
    tableName.clear();
    fields.clear();
    values.clear();
    wheres.clear();
    tableNames.clear();
}

QString DBManager::whereString()
{
    QStringList wheres;

    if (this->wheres.isEmpty())
        return "";

    for (int i=0; i<this->wheres.size(); i++)
        wheres << QString("%1=%2").arg(this->wheres.at(i).first, this->wheres.at(i).second);

    return " WHERE " + wheres.join(", ");
}
