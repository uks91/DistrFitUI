// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "sourcedatamanager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

SourceDataManager::SourceDataManager(QObject *parent)
    : QObject(parent)
    , lowerBoundary {Const::min_limit}
    , upperBoundary {Const::min_limit}
{

}

SourceDataManager &SourceDataManager::instance()
{
    static SourceDataManager singleton;
    return singleton;
}

void SourceDataManager::loadFromDB(const QString &value)
{    
    QStringList list = value.split(Const::srcDataSplitter, QString::SkipEmptyParts);
    prevSourceData = sourceData;
    sourceData.clear();
    QJsonDocument doc = QJsonDocument::fromJson(value.toUtf8());

    if (doc.isNull())
    {
        // Probably old version that doesn't suppose JSON
        for (int i=0; i<list.size(); i++)
        {
            QString tmp = list.at(i);
            sourceData << tmp.replace(",", ".").toDouble();
        }
        // TODO: it isn't needed!
        /*if (sourceData.size() > 7)
        {
            upperBoundary = sourceData.at(0);
            lowerBoundary = sourceData.at(2);
        }*/
        upperBoundary = Const::min_limit;
        lowerBoundary = Const::min_limit;
        emit sgnlSourceDataChanged();
        emit sgnlVariableModified(true);
    }
    else
    {
        QJsonObject obj = doc.object();
        if (obj["data"].isArray())
        {
            QJsonArray arr = obj["data"].toArray();
            sourceData.resize(arr.size());
            std::transform(arr.constBegin(), arr.constEnd(), sourceData.begin(), [](const QJsonValue& val)
            {
                return val.toDouble();
            });
        }

        auto lambda = [] (const QJsonValue &val)
        {
            return val.isDouble() ? val.toDouble() : Const::min_limit;
        };
        lowerBoundary = lambda (obj["lower"]);
        upperBoundary = lambda (obj["upper"]);
        emit sgnlSourceDataChanged();
        emit sgnlVariableModified(true);
    }
    /*if (!isInit && !isModified)
    {
        isModified = true;
        emit sgnlModified(isModified);
    }*/
}

QString SourceDataManager::saveToDB() const
{
    QJsonArray arr;
    std::for_each (sourceData.cbegin(), sourceData.cend(), [&arr] (const qreal x)
    {
        arr.append(QJsonValue(x));
    });

    QJsonObject obj;
    obj["data"] = arr;
    auto lambda = [&] (real x)
    {
        return x==Const::min_limit ? QJsonValue() : QJsonValue(x);
    };
    obj["lower"] = lambda(lowerBoundary);
    obj["upper"] = lambda(upperBoundary);
    QJsonDocument doc (obj);
    return doc.toJson();
}

real SourceDataManager::at(int i) const
{
    if (i >= size())
    {
        //qDebug () << "Out of size";
        return 0;
    }
    return sourceData[i];
}

void SourceDataManager::insert(int i)
{
    if (i > size())
    {
        //qDebug () << "Out of size";
        return ;
    }
    sourceData.insert(i, 0.0);
    emit sgnlSourceDataChanged();
    emit sgnlVariableModified(true);
}

void SourceDataManager::remove(int i)
{
    sourceData.removeAt(i);
    emit sgnlSourceDataChanged();
    emit sgnlVariableModified(true);
}

void SourceDataManager::setAt(int i, real value)
{
    if (i >= size())
    {
        qDebug () << "Out of size";
        return ;
    }
    sourceData[i] = value;
    emit sgnlSourceDataChanged();
    emit sgnlVariableModified(true);
}

void SourceDataManager::clearSourceData()
{
    //qDebug () << "Clearing source data ...";
    upperBoundary = Const::min_limit;
    lowerBoundary = Const::min_limit;
    sourceData.clear();
    emit sgnlSourceDataChanged();
    emit sgnlVariableModified(false);
    emit sgnlSourceDataCleared(); // CRUTCH!
    return ;
}

void SourceDataManager::slotSetLowerBoundary(real boundary)
{
    if (lowerBoundary == boundary)
        return;
    lowerBoundary = boundary;
    emit sgnlSourceDataChanged();
    emit sgnlVariableModified(true);
}

void SourceDataManager::slotSetUpperBoundary(real boundary)
{
    if (upperBoundary == boundary)
        return;
    upperBoundary = boundary;
    emit sgnlSourceDataChanged();
    emit sgnlVariableModified(true);
}
