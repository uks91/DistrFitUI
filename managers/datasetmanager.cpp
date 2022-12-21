// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "datasetmanager.h"
#include <QDateTime>
#include "dbmanager.h"
#include "resultsmanager.h"
#include "managers/errorhandler.h"

//!
//! \brief DatasetManager::DatasetManager
//! \param parent
//!
//!

QString boolVec2String(const BoolVec &value)
{
    QString out;
    for (const auto &elem : value)
    {
        if (elem)
            out += "1";
        else
            out += "0";
    }
    return out;
}


DatasetManager::DatasetManager(QObject *parent) :
    QObject(parent),
    dbmanager(DBManager::instance()),
    res {ResultsManager::instance()}
{
    calcCriteria = BoolVec (Const::countCriteria, true);
    calcDistrs   = BoolVec (Const::countDistrs, /*Options::getDistributions().size(),*/ false);

    calcMethods  = BoolVec (Const::countMethods, false);
    res.prepareMap(calcDistrs, calcMethods);
}


DatasetManager &DatasetManager::instance()
{
    static DatasetManager singleton;
    return singleton;
}

int DatasetManager::index() const
{
    auto found_iter = std::find_if(mList.cbegin(), mList.cend(), [this] (const Dataset& item)
    {
        return item.id == this->id();
    });

    int _index = std::distance(mList.cbegin(), found_iter);
    return _index >= mList.size() ? -1 : _index;
}

BoolVec DatasetManager::getCalcOption(CalcOption opt)
{
    switch (opt)
    {
        case CalcOption::Criteria:
            return calcCriteria;
        case CalcOption::Distribution:
            return calcDistrs;
        case CalcOption::Method:
            return calcMethods;
        default:
           return BoolVec ();
    }
}

void DatasetManager::setCalcOption(CalcOption opt, const BoolVec &value)
{
    BoolVec *vec = nullptr;

    switch (opt)
    {
        case CalcOption::Criteria:
            vec = &calcCriteria;
            break;
        case CalcOption::Distribution:
            vec =  &calcDistrs;
            break;
        case CalcOption::Method:
            vec = &calcMethods;
            break;
        default:
           return;
    }

    int sizeInit = vec->count();
    *vec = value;
    vec->resize(sizeInit);

    emit sgnlCalcOptionChanged(opt);
}

void DatasetManager::setCalcOption(CalcOption opt, const QString &value)
{
    BoolVec vec (value.size());

//    for (int i=0; i<vec.count(); i++)
    int i=0;
    for (const auto &ch : value)
    {
        vec[i] = ch == '1';
//        QChar ch = value.at(i);
//        if (i < value.size())
//            ch = value.at(i);

//        if (ch == '1')
//            vec[i] = true;
//        else
//            vec[i] = false;
        i++;
    }

    setCalcOption(opt, vec);
}

void DatasetManager::loadCalcOptions()
{

    VarTable tbl;
    dbmanager.prepareTables("tbl_sets");
    dbmanager.prepareFields("*");
    dbmanager.prepareWhere("id", id());
    if (!dbmanager.selectQuery(tbl))
    {
        auto& err {ErrorHandler::instance()};
        err.criticalMessage(tr("Unable to load CalcOptions: ")+dbmanager.lastError());
        //qCritical() << "Address";
        return;
    }

    if (tbl.count() < 1)
        return ;

    setCalcOption (CalcOption::Distribution, tbl.at(0)["distributions"].toString());
    setCalcOption (CalcOption::Method,       tbl.at(0)["methods"      ].toString());
//    setCalcOption (CalcOption::Criteria,     tbl.at(0)["criteria"     ].toString());
    res.prepareMap(calcDistrs, calcMethods);
}

void DatasetManager::saveCalcOptions()
{
    dbmanager.prepareTableName("tbl_sets");

//    dbmanager.prepare("criteria",      boolVec2String(calcCriteria));
    dbmanager.prepare("distributions", boolVec2String(calcDistrs));
    dbmanager.prepare("methods",       boolVec2String(calcMethods));

    dbmanager.prepareWhere("id", id());
    if (!dbmanager.updateQuery())
    {
        auto& err {ErrorHandler::instance()};
        err.criticalMessage(tr("Unable to save CalcOptions: ")+dbmanager.lastError());
        return;
    }
    res.prepareMap(calcDistrs, calcMethods);
}

bool DatasetManager::add()
{
    auto newDatasetName = [this]() -> QString
    {
        QString out = this->datasetDefaultName;
        QVector<QString> existentNames (this->mList.size());
        std::transform (this->mList.cbegin(),
                        this->mList.cend(),
                        existentNames.begin(), [](const Dataset &item){return item.name;});
        if (!existentNames.contains(out))
            return out;
        int i {0};
        do
        {
            out = this->datasetDefaultName + QString::number(++i);
        }
        while (existentNames.contains(out));

        return out;
    };

    int lastId = dbmanager.lastId("tbl_sets");
    Dataset dataset;
    dataset.id      = lastId;
    dataset.comment = "";
    dataset.name    = newDatasetName();
    dataset.date    = QDateTime::currentDateTime().toString(dateFormat);

    dbmanager.prepareTableName("tbl_sets");
    dbmanager.prepare("id",      dataset.id);
    dbmanager.prepare("name",    dataset.name);
    dbmanager.prepare("comment", dataset.comment);
    dbmanager.prepare("date",    dataset.date);

    if (!dbmanager.insertQuery())
    {

        return false;
    }
    mList.append(dataset);
    return true;
}

bool DatasetManager::update(const QString &name, const QString &comment)
{
    QVector<QString> existentNames (this->mList.size());
    std::transform (this->mList.cbegin(),
                    this->mList.cend(),
                    existentNames.begin(), [](const Dataset &item){return item.name;});

    if (existentNames.contains(name) && mCurrent.name != name)
    {
        _lastError = QString("Набор данных \"%1\" уже существует").arg(name);
        return false;
    }

    Dataset dataset = mCurrent;
    dataset.name    = name;
    dataset.comment = comment;

    dbmanager.prepareTableName("tbl_sets");
    dbmanager.prepare("name",    name);
    dbmanager.prepare("comment", comment);
    dbmanager.prepareWhere("id", mCurrent.id);

    if (name.isEmpty())
        return false;

    if ( !dbmanager.updateQuery() )
    {
        //qDebug () << "Address";
        return false;
    }

    for (auto it = mList.begin(); it != mList.end(); ++it)
        if (it->id == dataset.id)
            *it = dataset;
    return true;
}

bool DatasetManager::remove()
{
    dbmanager.prepareTableName("tbl_sets");
    dbmanager.prepareWhere("id", mCurrent.id);

    if (!dbmanager.deleteQuery())
    {
        return false;
    }

    dbmanager.prepareTableName("tbl_properties");
    dbmanager.prepareWhere("id_set", mCurrent.id);
    dbmanager.deleteQuery();

    for (auto it = mList.begin(); it != mList.end(); ++it)
        if (it->id == mCurrent.id)
        {
            mList.removeAt(it-mList.begin());
            break;
        }
    return true;
}

void DatasetManager::setDatasetItem(int index)
{
    if (index < 0 || index >= mList.count())
        return ;
    mCurrent = mList.at(index);
}

void DatasetManager::setDatasetForTest(int index)
{
    loadDatasets();
    if (mList.size()<=index)
    {
        return;
    }
    mCurrent = mList.at(index);
    unloadDatasets(true);
    return;

//    for (int i=0; i<mList.size(); i++)
//    {
//        if (mList.at(i).id == index)
//        {
//            mCurrent  = mList.at(i);
//        }
//    }
//    emit sgnlDataSetChanged(index);
    //!!!!!!!!!!!!!!!!    loadCalcOptions();
}

bool DatasetManager::isCurrentFixed()
{
    return false;
    return mCurrent.id == mFixed.id;
}

QStringList DatasetManager::namesList()
{
    QStringList list;

    for (auto it = mList.constBegin(); it != mList.constEnd(); ++it)
        list.append(it->name);
    return list;
}

void DatasetManager::loadDatasets()
{
    VarTable tbl;
    mFixed = mCurrent;
    dbmanager.prepareTables("tbl_sets");
    dbmanager.prepareFields("*");
    if (!dbmanager.selectQuery(tbl))
    {
        return;
    }

    mList.clear();

    for (int i=0; i<tbl.size(); i++)
    {
        Dataset tmp;
        tmp.comment = tbl.at(i)["comment"].toString();
        tmp.date    = tbl.at(i)["date"   ].toString();
        tmp.id      = tbl.at(i)["id"     ].toInt();
        tmp.name    = tbl.at(i)["name"   ].toString();
        mList.append(tmp);
    }
}

void DatasetManager::unloadDatasets(bool isAccepted)
{
    if (!isAccepted)
    {
        // Изменеие набора данных - ОТКЛОНЕНО
        //mCurrent = mFixed;
    }
    else
    {
        // Изменение набора данных - ПРИНЯТО
        loadCalcOptions();
        emit sgnlDataSetChanged();
    }
    mList.clear();
}

QString DatasetManager::lastError()
{
    if (_lastError.isEmpty())
        return dbmanager.lastError();

    QString out = _lastError;
    _lastError = QString();
    return out;
}

