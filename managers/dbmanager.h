#ifndef DBCONNECTOR_H
#define DBCONNECTOR_H

//#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include "../global.h"

class QSqlQuery;
class QSqlDatabase;

class DBManager /*: public QObject*/
{
    //Q_OBJECT
private:
    DBManager(/*QObject *parent = 0*/);

public:

    void  createTable      (const QString &tableName, const QStringList &fields);
    bool  deleteQuery      ();
    void  init             ();
    bool  insertQuery      ();
    int   lastId           (const QString &tableName);

    void  prepare          (const QString &fieldName, int value);
    void  prepare          (const QString &fieldName, double value);
    void  prepare          (const QString &fieldName, const QString &value);
    void  prepareFields    (const QString &fieldName);
    void  prepareTableName (const QString &tableName);
    void  prepareTables    (const QString &tableName);
    void  prepareWhere     (const QString &fieldName, int value);
    void  prepareWhere     (const QString &fieldName, double value);
    void  prepareWhere     (const QString &fieldName, const QString &value);
    bool  selectQuery      (VarTable &tbl);
    bool  updateQuery      ();

    QString lastError () const;
    static DBManager& instance();

private:

    QSqlDatabase db;
    bool         isInit;

    QSqlQuery    query;
    QString      tableName;
    QStringList  tableNames;
    QStringList  fields;
    QStringList  values;
    QList<QPair<QString, QString>> wheres;

    void clear ();
    QString whereString();

};


#endif // DBCONNECTOR_H
