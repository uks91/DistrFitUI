#ifndef DATASETMANAGER_H
#define DATASETMANAGER_H

#define QT_MESSAGELOGCONTEXT

#include <QObject>
#include "global.h"

extern template class QVector<QMap<QString, QVariant>>;
extern template class QVector<bool>;
extern template class QVector<double>;
extern template class QVector<int>;

struct Dataset
{
    QString   comment;
    QString date;
    int       id;
    QString   name;
};

class DBManager;
class ResultsManager;

//!
//! \brief The DatasetManager class
//!

class DatasetManager : public QObject
{
    Q_OBJECT
public:
    static DatasetManager& instance ();


    //INLINE FUNCTIONS
    inline QString comment () const
    {
        return mCurrent.comment;
    }

    inline QString date () const
    {
        return mCurrent.date;
    }

    inline int id () const
    {
        return mCurrent.id;
    }

    int index () const;

    inline QString name () const
    {
        return mCurrent.name;
    }

    // NON-INLINE FUNCTIONS

    BoolVec  getCalcOption   (CalcOption opt);
    void     setCalcOption   (CalcOption opt, const BoolVec &value);
    void     setCalcOption   (CalcOption opt, const QString &value);
    void     loadCalcOptions ();
    void     saveCalcOptions ();

    bool add();
    bool update(const QString &name, const QString &comment);
    bool remove();

    void setDatasetItem(int index);
    void setDatasetForTest(int index);

    bool        isCurrentFixed ();
    QStringList namesList ();
    void        loadDatasets     ();
    void        unloadDatasets (bool isAccepted);

    QString lastError ();
signals:
    void sgnlCalcOptionChanged (CalcOption);
    void sgnlDataSetChanged ();

public slots:

private:
    DBManager      &dbmanager;
    ResultsManager &res;
    Dataset        mCurrent;
    Dataset        mFixed;
    QList<Dataset> mList;

    BoolVec calcCriteria;
    BoolVec calcDistrs;       // Распределения, которые будут анализироваться при расчёте
    BoolVec calcMethods;      // Методы определения параметров распределений

    const QString datasetDefaultName = "Набор данных";
    const QString dateFormat         = "yyyy-MM-dd hh:mm:ss";
    const QString propDefaultName    = "Характеристика";
    QString _lastError;

    DatasetManager(QObject *parent = nullptr);
};

#endif // DATASETMANAGER_H
