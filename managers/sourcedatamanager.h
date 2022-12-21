#ifndef SOURCEDATAMANAGER_H
#define SOURCEDATAMANAGER_H

#include <QObject>
#include "global.h"
#include <climits>

extern template class QVector<QMap<QString, QVariant>>;
extern template class QVector<bool>;
extern template class QVector<double>;
extern template class QVector<int>;

class SourceDataManager : public QObject
{
    Q_OBJECT
public:
    static SourceDataManager& instance ();
    void loadFromDB (const QString &value);
    QString saveToDB () const;
    inline realVec getSourceData () const {return sourceData;}

    real at (int i) const;
    void insert (int i);
    void remove (int i);
    void setAt (int i, real value);
    int size () const {return sourceData.size();}

    real getLowerBoundary () const {return lowerBoundary;}
    real getUpperBoundary () const {return upperBoundary;}

    void clearSourceData();

//    constexpr static real min_limit = std::numeric_limits<double>::lowest();

signals:
    void sgnlVariableModified (bool);
    void sgnlSourceDataChanged();
    void sgnlSourceDataCleared();

public slots:
    void slotSetLowerBoundary (real boundary);
    void slotSetUpperBoundary (real boundary);

private:
    explicit SourceDataManager(QObject *parent = nullptr);

    realVec sourceData;
    realVec prevSourceData;

    real lowerBoundary;
    real upperBoundary;
};

#endif // SOURCEDATAMANAGER_H
