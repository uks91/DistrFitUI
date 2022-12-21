#ifndef SOLVER_H
#define SOLVER_H

#include <QThread>
#include <QFuture>
#include <fitting.h>
#include "managers/resultsmanager.h"
#include "task.h"

class VariableManager;
class DatasetManager;
class SourceDataManager;

class Solver : public QThread
{
    Q_OBJECT
public:
    explicit Solver(QObject *parent = nullptr);
    ~Solver ();

    QString mathString   () const;
    void    setData      (const realVec &value);
    void    setHypothesis(const Fitting::Hypothesis hyp);
    void    setDigits    (uint digits);
    void    setNumIters  (const uint numIters);
    void    setNumItersForInrange (const uint numIters);
    void    setThreadCount (int value);

signals:
    void sgnlUpdateProgress(int);
    void sgnlWaitingAll ();
    void sgnlInProcess (ResultsIndex, ResultsManager::ResultType);


public slots:
    void slotSourceDataChanged();
    void slotCancel ();

private:
//    Fitting fit;
    VariableManager &mgr;
    DatasetManager &dsm;
    ResultsManager &res;
    SourceDataManager &src;
//    uint numIters;
//    bool running;
    QString math;

    array1 data;
    Fitting::Hypothesis hyp;
    QFuture<void> future;
    int threadCount;

//    void                 calculate   (AbstractDistribution *d, QString key);
    bool                 check       () const;
//    QJsonObject          nullObject  ();
//    QString              numArr2Str  (const QVector<double> &in);
//    QString              numArr2Str  (const array1 &in);
    void                 run         ();



};

#endif // SOLVER_H
