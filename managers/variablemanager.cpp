// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "variablemanager.h"
#include "dbmanager.h"
#include "datasetmanager.h"
#include "resultsmanager.h"
#include "sourcedatamanager.h"


//!
//!
//! \brief DataManager::DataManager #################################################################
//!
//!

VariableManager::VariableManager(QObject *parent)
    : QObject(parent)
    , dbmanager (DBManager::instance())
    , dsm {DatasetManager::instance()}
    , res {ResultsManager::instance()}
    , src {SourceDataManager::instance()}
    , isModified {false}

{
    auto lambda = [&](bool modified)
    {
        isModified = modified;
        emit sgnlModified(isModified);
    };

    connect (&dsm, &DatasetManager::sgnlDataSetChanged, this, &VariableManager::slotDatasetChanged);
    connect (&res, &ResultsManager::sgnlModified, lambda);
    connect (&src, &SourceDataManager::sgnlVariableModified, lambda);
}


//*****
//*****
//***** Source Data & Results
//*****
//*****

VariableManager &VariableManager::instance()
{
    static VariableManager singleton;
    return singleton;
}

void VariableManager::setVariableName(const QString &name)
{
    prevVariableName = curVariableName;
    curVariableName = name;
    if (!isInit && !isModified)
    {
        isModified = true;
        emit sgnlModified(isModified);
    }
}

int VariableManager::datasetId() const {return dsm.id();}

//*****
//*****
//***** Property
//*****
//*****

int VariableManager::id() const
{
    if (curIndex < 0 || curIndex >= propList.count())
        return -1;
    return propList.at(curIndex).id;
}

QStringList VariableManager::namesList() const
{
    QStringList out;
    for (const auto &elem : propList)
        out.push_back(elem.name);
    return out;
}

bool VariableManager::addProperty()
{
    Prop tmp;
    tmp.dataset = dsm.id();
    tmp.id      = dbmanager.lastId("tbl_properties");
    tmp.name    = propDefaultName;
    //qDebug () << "Dsm id is: " << dsm.id();

    dbmanager.prepareTableName("tbl_properties");
    dbmanager.prepare("id",     tmp.id);
    dbmanager.prepare("id_set", tmp.dataset);
    dbmanager.prepare("name",   tmp.name);

    if (!dbmanager.insertQuery())
    {
        //qDebug () << "Address";
        return false;
    }
    propList.push_back(tmp);
    // TODO: add the function for changing curIndex and sending signal
    curIndex = propList.count()-1;
    emit sgnlPropListChanged(curIndex);
    return true;
}

int VariableManager::changeProperty(int index)
{
    int previndex = curIndex;
    if (!isCurIndexValid())
        return previndex;

    curIndex = index;

//    if (propertyId < 0)
//        return;

    VarTable tbl;
    dbmanager.prepareTables("tbl_properties");
    dbmanager.prepareFields("*");
    dbmanager.prepareWhere("id", this->id());
    if (!dbmanager.selectQuery(tbl))
    {
        qCritical() << "Address";
        return -1;
    }

    if (tbl.size() < 1)
        return -1;

    isInit = true;
    setVariableName (tbl.at(0)["name"   ].toString());
//    setSourceData   (tbl.at(0)["data"   ].toString());
//    res.setResults      (tbl.at(0)["results"].toString(), 0);
    res.loadFromDB(tbl.at(0)["results"].toString());
    src.loadFromDB(tbl.at(0)["data"   ].toString());

    isInit = false;

    prevVariableName = QString();
    //prevResults  = QString();
    prevSourceData.clear();

    emit sgnlPropertyIdChanged(propList.at(curIndex).id);
    isModified = false;
    emit sgnlModified(isModified);
    return previndex;
}

bool VariableManager::deleteProperty()
{
    // Удаляем параметр
    if (!isCurIndexValid())
        return false;

    dbmanager.prepareTableName("tbl_properties");
    dbmanager.prepareWhere("id", id());
    if (!dbmanager.deleteQuery())
    {
        //qDebug () << "Address";
        return false;
    }
    propList.removeAt(curIndex);
    if (curIndex >= propList.count())
        curIndex = propList.count()-1;
    sgnlPropListChanged(curIndex);
    return true;
}

bool VariableManager::saveProperty()
{
    // Сохраняем значение параметра, результаты стат анализа

    if (id() < 0)
        return false;

    QStringList dataStringList;
//    for (int i=0; i<sourceData.size(); i++)
    for (const auto &elem : sourceData)
        dataStringList << QString::number(elem);


    dbmanager.prepareTableName("tbl_properties");
    dbmanager.prepare("name",    curVariableName);
//    dbmanager.prepare("data",    dataStringList.join(";"));
    dbmanager.prepare("data",    src.saveToDB());
    dbmanager.prepare("results", res.getResults());
    dbmanager.prepareWhere("id", id());

//    qDebug () << src.saveToDB();

    if ( !dbmanager.updateQuery() )
    {
        //qDebug () << "Address";
        return false;
    }

    propList[curIndex].name = curVariableName;

    sgnlPropListChanged(curIndex);

    isModified = false;
    emit sgnlModified(isModified);

    return true;

}

void VariableManager::slotDatasetChanged()
{
//    loadCalcOptions();


    int id = dsm.id();
    //qDebug () << "DataManager::slotDatasetChanged() : id is: " << id << id2;
    if (id <= 0)
        return ;

    VarTable tbl;

    dbmanager.prepareTables("tbl_properties");
    dbmanager.prepareFields("*");
    dbmanager.prepareWhere("id_set", id);
    if (!dbmanager.selectQuery(tbl))
    {
        qCritical () << "Address";
        return ;
    }

    propList.clear();

    if (tbl.size() < 1)
    {
        curIndex = -1;
        emit sgnlPropListChanged(curIndex);
        src.clearSourceData();
        return ;
    }

    for (const auto &elem : tbl)
    {
        Prop tmp;
        tmp.id      = elem["id"  ].toInt();
        tmp.name    = elem["name"].toString();
        tmp.dataset = dsm.id();
        propList.append(tmp);
//        qDebug () << tmp.id << tmp.name;
    }
    curIndex = 0; // Sets the current property index to begin (0)
    emit sgnlPropListChanged(curIndex);

}

//QString DataManager::data2String()
//{
//    QStringList lst;
//    for (int i=0; i<sourceData.size(); i++)
//        lst << QString::number(sourceData.at(i));
//    return lst.join(";");
//}
