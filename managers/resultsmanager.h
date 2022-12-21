#ifndef RESULTSMANAGER_H
#define RESULTSMANAGER_H

#include <QObject>
#include <QMap>
#include "global.h"
#include "fitting.h"
#include <QMetaType>

struct SingleResult
{
    realVec pars;
    realVec pvalues;
    realVec criteria;
    real    inrange;
    real    inrangeMean;
    real    inrangeSigma;
    real    inrangeBottom;
    bool    isParsDefined{false};
};

class ResultsIndex
{
public:
    ResultsIndex (const ResultsIndex& other);
    ResultsIndex& operator =(const ResultsIndex& other);
    ResultsIndex (const int distrId=-1, const int methodId=-1);
    ResultsIndex (const QString& distrName, const QString& methodName);
    ~ResultsIndex() = default;

    static int     convertDistrName2Id  (const QString& distrName);
    static QString convertDistrId2Name  (const uint      distrId);
    static int     convertMethodName2Id (const QString& distrName);
    static QString convertMethodId2Name (const uint methodId);

    inline bool isValid () const {return valid;}

    int  getDistrId  () const {return distrId;}
    int  getMethodId () const {return methodId;}
    QString  getDistrName  () const {return convertDistrId2Name (distrId );}
    QString  getMethodName () const {return convertMethodId2Name(methodId);}

    void setDistrId  (const int id);
    void setDistrId  (const QString& distrName);
    void setMethodId (const int id);
    void setMethodId (const QString& methodName);

//    bool operator == (const ResultsIndex& other);
    bool operator != (const ResultsIndex& other);
    bool operator <  (const ResultsIndex& other);
    bool operator >  (const ResultsIndex& other);
private:
    void validate () {validate(distrId, methodId);}
    void validate (const int distrId, const int methodId);

    int distrId;
    int methodId;
    bool valid;

    const QList<QStringList>& distrs ;
    const QList<QStringList>& methods;
};

Q_DECLARE_METATYPE(ResultsIndex)


bool operator == (const ResultsIndex& first, const ResultsIndex& second);
//bool operator != (const ResultsIndex& first, const ResultsIndex& second);
bool operator <  (const ResultsIndex& first, const ResultsIndex& second);
//bool operator >  (const ResultsIndex& first, const ResultsIndex& second);

using ResultsMap = QMap<ResultsIndex, SingleResult>;

class ResultsManager : public QObject
{
    Q_OBJECT
public:
    enum ResultType {DISTR_PARS, PROB_INRANGE, PROB_INRANGE_MEAN, PROB_INRANGE_SIGMA, PROB_INRANGE_BOTTOM, KOLMOGOROV, CRAMER_MISES, ANDERSON};
    Q_ENUM (ResultType)
    static ResultsManager& instance();
    void  loadFromDB (const QString& dbString);

    ResultsIndex getIndexByRow (int row) const;
    SingleResult getResultByRow (int row) const;
    SingleResult getResultByIndex (const ResultsIndex& index) const;
    int          getRowByIndex  (const ResultsIndex& index) const;

    void clear ();

    QList<ResultsIndex> getIndexes () const {return map.keys();}
    int getMapSize () const {return map.size();}
    QString getResults () const;
    void prepareMap (const BoolVec& distrs, const BoolVec& methods);
//    void setResults (const QString &value, int send);


    void setDistrPars   (const ResultsIndex& idx, const realVec& pars);
    void setProbInrange (const ResultsIndex& idx, const real inrange);
    void setFitResults  (const ResultsIndex& idx, Fitting::Method critType, const real critValue, const real pvalue);

    void setProbInrangeMean   (const ResultsIndex& idx, const real mean);
    void setProbInrangeSigma  (const ResultsIndex& idx, const real sigma);
    void setProbInrangeBottom (const ResultsIndex& idx, const real bottom);

signals:
    void sgnlAllResultsChanged     ();
    void sgnlMapRestructed ();
    void sgnlModified (bool);
    void sgnlResultsChanged (ResultType, ResultsIndex);

public slots:

private:
    explicit ResultsManager(QObject *parent = nullptr);

    void parseJsonItem (const QString& key, const QJsonObject& jsonObj);

    ResultsMap map;
    QString    results;
    QString    prevResults;
    bool       isModified;
};

QDebug operator<<(QDebug deb, const SingleResult& res);
QDebug operator<< (QDebug deb, const ResultsIndex& idx);

#endif // RESULTSMANAGER_H
