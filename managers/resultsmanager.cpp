// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "resultsmanager.h"
#include "options.h"
#include <QtDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <algorithm>

ResultsIndex::ResultsIndex(const ResultsIndex &other)
    : ResultsIndex(other.distrId, other.methodId)
{

}

ResultsIndex &ResultsIndex::operator =(const ResultsIndex &other)
{
    // WARNING: all paths through this function will call itself
    *this = other;
    return *this;
}

ResultsIndex::ResultsIndex(const int distrId, const int methodId) :
    distrId {distrId},
    methodId{methodId}
  , distrs  {Options::getDistributions()}
  , methods {Options::getMethods()}
{
    validate();
}

ResultsIndex::ResultsIndex(const QString &distrName, const QString &methodName) :
    ResultsIndex(convertDistrName2Id(distrName), convertMethodName2Id(methodName))
{

}

int ResultsIndex::convertDistrName2Id(const QString &distrName)
{
    QString name = distrName;
    name = name.replace("Distr", "");
    auto list = Options::getDistributions();
    for (auto iter=list.cbegin(); iter != list.cend(); ++iter)
        if (iter->at(Options::LATIN)==name)
            return iter-list.cbegin();

    return -1;
}

QString ResultsIndex::convertDistrId2Name(const uint distrId)
{
    auto list = Options::getDistributions();
    if (distrId >= static_cast<uint>(list.size()))
        return QString();
    else
        return list[distrId][Options::LATIN];
}

int ResultsIndex::convertMethodName2Id(const QString &methodName)
{
    auto list = Options::getDistributions();
    for (auto iter=list.cbegin(); iter != list.cend(); ++iter)
        if (iter->at(Options::LATIN)==methodName)
            return iter-list.cbegin();

    return -1;
}

QString ResultsIndex::convertMethodId2Name(const uint methodId)
{
    auto list = Options::getMethods();
    if (methodId >= static_cast<uint>(list.size()))
        return QString();
    else
        return list[methodId][Options::LATIN];
}

void ResultsIndex::setDistrId(const int id)
{
    distrId = id;
    validate ();
}

void ResultsIndex::setDistrId(const QString &distrName)
{
    distrId = convertDistrName2Id(distrName);
    validate();
}

void ResultsIndex::setMethodId(const int id)
{
    methodId = id;
    validate ();
}

void ResultsIndex::setMethodId(const QString &methodName)
{
    methodId = convertMethodName2Id(methodName);
    validate();
}
/*
bool ResultsIndex::operator ==(const ResultsIndex &other)
{
    return this->distrId  == other.distrId &&
           this->methodId == other.methodId;
}*/

bool ResultsIndex::operator !=(const ResultsIndex &other)
{
    return !(*this == other);
}

bool ResultsIndex::operator <(const ResultsIndex &other)
{
    // WARNING: all paths through this function will call itself
    return *this < other;
}

bool ResultsIndex::operator >(const ResultsIndex &other)
{

    return !(*this < other);
}

void ResultsIndex::validate(const int distrId, const int methodId)
{
    valid = (distrId  >= 0 && distrId  < distrs.size() )
         && (methodId >= 0 && methodId < methods.size());
}

bool operator ==(const ResultsIndex &first, const ResultsIndex &second)
{
    return first.getDistrId()  == second.getDistrId() &&
           first.getMethodId() == second.getMethodId();
}

bool operator <(const ResultsIndex &first, const ResultsIndex &second)
{
    int firstID = first.getDistrId();
    int secondID = second.getDistrId();
    if (firstID == secondID)
    {
        return first.getMethodId() < second.getMethodId();
    }
    else
        return firstID < secondID;
}


#ifdef QDEBUG_H
QDebug operator<<(QDebug deb, const SingleResult& res)
{
    QStringList list;

    for (const auto& elem: res.pars)
        list << QString::number(elem);
    deb << QString("SingleResult {pars (%1),").arg(list.join(";")).toStdString().data();
    list.clear();

    for (const auto& elem: res.criteria)
        list << QString::number(elem);
    deb << QString("crit (%1),").arg(list.join(";")).toStdString().data();
    list.clear();

    for (const auto& elem: res.pvalues)
        list << QString::number(elem);
    deb << QString("pval (%1)}").arg(list.join(";")).toStdString().data();
    list.clear();

    return deb;
}

QDebug operator<< (QDebug deb, const ResultsIndex& idx)
{
    deb << QString("ResultsIndex{%1_%2, %3, %4}")
           .arg(idx.getDistrId())
           .arg(idx.getMethodId())
           .arg(idx.getDistrName())
           .arg(idx.getMethodName())
           .toStdString().data();
    return deb;
}
#endif

//!
//! \brief ResultsManager::ResultsManager
//! \param parent
//!

ResultsManager::ResultsManager(QObject *parent) :
    QObject(parent),
    isModified(false)
{
    qRegisterMetaType<ResultType>("ResultType");
}

ResultsManager &ResultsManager::instance()
{
    static ResultsManager singleton;
    return singleton;
}

void ResultsManager::loadFromDB(const QString &dbString)
{
    clear();
    auto res = dbString;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    QJsonObject json = jsonDoc.object();
    for (auto iter = json.constBegin(); iter != json.constEnd(); ++iter)
    {
        QJsonValue val = iter.value();
        if (!val.isObject())
            continue;

        parseJsonItem(iter.key(), val.toObject());
    }
    emit sgnlAllResultsChanged();
    //qDebug () << "LoadFromDB" << map;
}

ResultsIndex ResultsManager::getIndexByRow(int row) const
{
    if (row < 0 || row >= map.size())
    {
        //qDebug () << "Row is out of size in getIndexByRow()!";
        return ResultsIndex{};
    }
    return (map.cbegin()+row).key();
}

SingleResult ResultsManager::getResultByRow(int row) const
{
    if (row < 0 || row >= map.size())
    {
        //qDebug () << "Row is out of size in getResultByRow()!";
        return SingleResult{};
    }
    return (map.cbegin()+row).value();
}

SingleResult ResultsManager::getResultByIndex(const ResultsIndex &index) const
{
    if (!map.contains(index))
        return SingleResult{};

    return map[index];
}

int ResultsManager::getRowByIndex(const ResultsIndex &index) const
{
    int row {0};
    auto iter = map.cbegin();
    for (; iter != map.cend(); ++iter)
    {
        if (iter.key() == index)
            return row;
        row++;
    }
    return 0;
}

void ResultsManager::clear()
{
    for (auto &elem : map)
    {
        elem = SingleResult{};
        elem.criteria.resize(3);
        elem.pvalues .resize(3);
    }
    emit sgnlAllResultsChanged();
}

QString ResultsManager::getResults() const
{
//    QJsonDocument jsonDoc;
    QJsonObject   mainObj;
    auto iter = map.cbegin();

    using std::for_each;

    for (; iter!=map.cend(); ++iter)
    {
        auto key = iter.key();
        auto value = iter.value();
        QJsonObject obj;
//        iter.key()

        obj["inrange"]        = QJsonValue (value.inrange);
        obj["inrange_mean"]   = QJsonValue (value.inrangeMean);
        obj["inrange_sigma"]  = QJsonValue (value.inrangeSigma);
        obj["inrange_bottom"] = QJsonValue (value.inrangeBottom);
        QJsonArray jsonArr;
        for_each(value.pars.cbegin(), value.pars.cend(), [&] (const real x)
        {
            jsonArr.append(QJsonValue(x));
        });
        obj["pars"] = jsonArr;
        auto addToJsonObj = [&obj, i=0] (const realVec &vec, const QString &prefix) mutable
        {
            for_each (vec.cbegin(), vec.cend(), [&] (const real x)
            {
                obj[QString("%1%2").arg(prefix).arg(i++)] = x;
            });
            i=0;
        };
        addToJsonObj (value.pvalues, "pval");
        addToJsonObj (value.criteria, "s");

        auto strKey = QString("%1_%2").arg(key.getDistrId()).arg(key.getMethodId());
        mainObj[strKey] = obj;
    }
    QJsonDocument doc (mainObj);
    return doc.toJson();
}

void ResultsManager::prepareMap(const BoolVec &distrs, const BoolVec &methods)
{
//    map.clear();
    SingleResult zeroResult;
    int dId {0};
    int mId {0};
//    qDebug () << distrs << methods;
    for (auto distrIter=distrs.cbegin(); distrIter != distrs.cend(); ++distrIter)
    {
        dId = distrIter-distrs.cbegin();
        for (auto methodIter=methods.cbegin(); methodIter != methods.cend(); ++methodIter)
        {
            mId = methodIter-methods.cbegin();
            ResultsIndex res (dId, mId);
            if (!(*distrIter && *methodIter))
                map.remove(res);
            else
                if (!map.contains(res))
                    map[res] = zeroResult;
        }
    }

    emit sgnlMapRestructed();
    //qDebug () << "PrepareMap" << map << endl << endl;
}

//void ResultsManager::setResults(const QString &value, int send)
//{
//    return;
//    prevResults = results;
//    results = value;
//    emit sgnlAllResultsChanged(send);

//    if (/*!isInit && */!isModified)
//    {
//        isModified = true;
//        emit sgnlModified(isModified);
//    }
//}

void ResultsManager::setDistrPars(const ResultsIndex &idx, const realVec &pars)
{
    if (!map.contains(idx))
        return;
    map[idx].pars = pars;
    map[idx].isParsDefined = true;
    emit sgnlResultsChanged(DISTR_PARS, idx);
    emit sgnlModified(true);
}

void ResultsManager::setProbInrange(const ResultsIndex &idx, const real inrange)
{
    if (!map.contains(idx))
        return;
    map[idx].inrange = inrange;
    emit sgnlResultsChanged (PROB_INRANGE, idx);
    emit sgnlModified(true);
}

void ResultsManager::setFitResults(const ResultsIndex &idx, Fitting::Method critType, const real critValue, const real pvalue)
{
    if (!map.contains(idx))
        return;

    ResultType resType;
    int index = 0;
    switch (critType)
    {
        case Fitting::Kolmogorov:
            resType = ResultType::KOLMOGOROV;
            index = 0;
            break;
        case Fitting::CramerVonMises:
            resType = ResultType::CRAMER_MISES;
            index = 1;
            break;
        case Fitting::AndersonDarling:
            resType = ANDERSON;
            index = 2;
            break;
        default:
            return;
    }

//    map[idx].criteria.resize(3);
//    map[idx].pvalues .resize(3);
    map[idx].criteria[index] = critValue;
    map[idx].pvalues [index] = pvalue;
    emit sgnlResultsChanged(resType, idx);
    emit sgnlModified(true);
}

void ResultsManager::setProbInrangeMean(const ResultsIndex &idx, const real mean)
{
    if (!map.contains(idx))
        return;
    map[idx].inrangeMean = mean;
    emit sgnlResultsChanged (PROB_INRANGE_MEAN, idx);
    emit sgnlModified(true);
}

void ResultsManager::setProbInrangeSigma(const ResultsIndex &idx, const real sigma)
{
    if (!map.contains(idx))
        return;
    map[idx].inrangeSigma = sigma;
    emit sgnlResultsChanged (PROB_INRANGE_SIGMA, idx);
    emit sgnlModified(true);
}

void ResultsManager::setProbInrangeBottom(const ResultsIndex &idx, const real bottom)
{
    if (!map.contains(idx))
        return;
    map[idx].inrangeBottom = bottom;
    emit sgnlResultsChanged (PROB_INRANGE_BOTTOM, idx);
    emit sgnlModified(true);
}

void ResultsManager::parseJsonItem(const QString &key, const QJsonObject &jsonObj)
{
    /* jsonObj должен иметь следующий вид:
     * {
     *    "inrange": double,
     *    "pars":"double;double;..."
     *    "pvalX":double,
     *    "sX" : double
     * }
     *
     */

    QStringList keys = key.split("_");
    if (keys.size() != 2)
        return;

    bool chk1, chk2;
    int distrId  = keys.at(0).toInt(&chk1);
    int methodId = keys.at(1).toInt(&chk2);

    if (!(chk1 || chk2))
        return;

    auto index = ResultsIndex(distrId, methodId);
    if (!index.isValid() || !map.contains(index))
        return;

    SingleResult res;
    res.inrange = jsonObj["inrange"].toDouble();
    res.inrangeBottom = jsonObj["inrange_bottom"].toDouble();
    res.inrangeSigma  = jsonObj["inrange_sigma"].toDouble();
    res.inrangeMean   = jsonObj["inrange_mean"].toDouble();
    for (int i=0; i<3; i++)
    {
        QString pvalKey = QString("pval%1").arg(i);
        QString critKey = QString("s%1").arg(i);

        res.criteria.append(jsonObj[critKey].toDouble());
        res.pvalues. append(jsonObj[pvalKey].toDouble());
    }
    if (jsonObj["pars"].isString())
    {
        // OldVariant
        auto parsStr = jsonObj["pars"].toString().split(";");
        for (const QString& par : parsStr)
        {
            bool b;
            real value = par.toDouble(&b);
            if (b)
                res.pars.append(value);
            else
                return;
        }
    }
    else if (jsonObj["pars"].isArray())
    {
        auto& val = res.pars;
        auto jsonArray = jsonObj["pars"].toArray();
        val.resize(jsonArray.size());
        std::transform (jsonArray.begin(), jsonArray.end(), val.begin(), [] (const QJsonValue &x)
        {
            return x.toDouble();
        });
    }
    map[index] = res;
}


