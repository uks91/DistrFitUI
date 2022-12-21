#ifndef VARIABLEMANAGER_H
#define VARIABLEMANAGER_H

#include <QObject>
#include "../global.h"

class DBManager;
class DatasetManager;
class ResultsManager;
class SourceDataManager;

extern template class QVector<QMap<QString, QVariant>>;
extern template class QVector<bool>;
extern template class QVector<double>;
extern template class QVector<int>;

//!
//!
//! \brief The DataManager class
//!
//!

struct Prop
{
    QString name;
    int     id;
    int     dataset;
};

//!
//! \brief The DataManager class
//!

class VariableManager : public QObject
{
    Q_OBJECT
public:
    static VariableManager& instance ();

    //***** Calculation Options


    //***** Source Data & Results
    inline QString  getVariableName () const {return curVariableName;}
    //inline realVec  getSourceData   () const {return sourceData; }
public slots:
    void     setVariableName (const QString &name);
    //void     setSourceData   (const QString &value);

    //***** Property Managment
public:

    int  datasetId () const;
    int  id() const;
    inline bool isCurIndexValid () const {return !(curIndex < 0 || curIndex >= propList.count()); }
    QStringList namesList () const;

    bool addProperty();
    int  changeProperty (int index);
    bool deleteProperty();
    bool saveProperty();


signals:


    void sgnlPropertyIdChanged  (int);
//    void sgnlResultsChanged     (int);
    //void sgnlSourceDataChanged  ();

    void sgnlPropListChanged (int);
    void sgnlModified        (bool);

public slots:

    void            slotDatasetChanged ();
private:
    DBManager &dbmanager;
    DatasetManager &dsm;
    ResultsManager &res;
    SourceDataManager &src;
    bool isModified;

    realVec sourceData;
    realVec prevSourceData;

    QString curVariableName;
    QString prevVariableName;

    int      curIndex;

    QList<Prop> propList;

    bool isInit;
    const QString propDefaultName = "Величина";
    VariableManager(QObject *parent = 0);

};

#endif // VARIABLEMANAGER_H
