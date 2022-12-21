// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "solver.h"
#include <cmath>
#include <distributions.h>
#include <QtConcurrent>
#include "managers/variablemanager.h"
#include "managers/datasetmanager.h"
#include "managers/sourcedatamanager.h"

// WARNING: CRUTCH!!
AbstractDistribution* chooseById(int id)
{
    switch (id)
    {
        case 0:
            return new NormalDistr();
        case 1:
            return new LognormalDistr ();
        case 2:
            return new WeibullDistr ();
        case 3:
            return new LogisticDistr ();
        case 4:
            return new MoyalDistr ();
        case 5:
            return new ExtremeValueDistr ();
//        case 6:
//            return new GumbelDistr ();
        default:
            return nullptr;
    }
}

Solver::Solver(QObject *parent) /*: QObject(parent)*/
    : QThread(parent)
    , mgr {VariableManager::instance()}
    , dsm {DatasetManager::instance()}
    , res {ResultsManager::instance()}
    , src {SourceDataManager::instance()}
    , threadCount {QThread::idealThreadCount()}
{

//    connect (&mgr, &VariableManager::sgnlSourceDataChanged, this, &Solver::slotSourceDataChanged);
    connect(&src, &SourceDataManager::sgnlSourceDataChanged, this, &Solver::slotSourceDataChanged);
}

Solver::~Solver()
{
    future.cancel();
    future.waitForFinished();
    quit();
}

QString Solver::mathString() const
{
    QString math = "kk = {";
    QStringList lst;
    for (size_t i=0; i<data.size(); i++)
    {
        lst << QString::number(data.at(i));
    }

    math += lst.join(", ") + "};\nMin[kk]\nMax[kk]\ndistr = {};\n";
    lst.clear();

    math += this->math;
    return math;
}


void Solver::slotSourceDataChanged()
{
//    data = mgr.getSourceData().toStdVector();
    // NOTE: It's seemed to be unused function
//    data = src.getSourceData().toStdVector();

}

void Solver::slotCancel()
{
    future.cancel();
    future.waitForFinished();
    quit();
    emit sgnlUpdateProgress(100);
}

void Solver::setData(const realVec &value)
{
    // NOTE: It's seemed to be unused function
    if (isRunning())
    {
        return;
    }
    data = value/*.toStdVector()*/;
}

void Solver::setHypothesis(const Fitting::Hypothesis hyp)
{
    //fit.setHypothesis(hyp);
    if (isRunning())
    {
        //qDebug () << "Running!";
        return;
    }
    CriteriaTask::setHypothesis(hyp);
    this->hyp = hyp;
}

void Solver::setDigits(uint digits)
{
    if (isRunning())
    {
        //qDebug () << "Running!";
        return;
    }
    digits = digits>5 ? 5 : digits;
    CriteriaTask::setRoundDigits(digits);
}

void Solver::setNumIters(const uint numIters)
{
    if (isRunning())
    {
        //qDebug () << "Running!";
        return;
    }
    CriteriaTask::setNumIters(numIters);
}

void Solver::setNumItersForInrange(const uint numIters)
{
    if (isRunning())
    {
        //qDebug () << "Running!";
        return;
    }
    InrangeTask::setNumIters(numIters);
}

void Solver::setThreadCount(int value)
{
    threadCount = value;
}


bool Solver::check() const
{
    return data.size() >=2;
}

//QString Solver::numArr2Str(const QVector<double> &in)
//{
//    QStringList lst;
//    for (int i=0; i<in.size(); i++)
//    {
//        lst << QString::number(in.at(i));
//    }
//    return lst.join(", ");
//}

//QString Solver::numArr2Str(const array1 &in)
//{
//    return numArr2Str(QVector<double>::fromStdVector(in));
//}

void Solver::run()
{
    data = src.getSourceData()/*.toStdVector()*/;

    if (data.size() < 2)
    {
        //qDebug () << "Unchecked!";
        return;
    }

    res.clear();
    emit sgnlUpdateProgress(0);
    emit sgnlWaitingAll();
    int step {0};

    QList<AbstractTask*> tasks;
    auto indexes = res.getIndexes();

    for (const auto &index: indexes)
    {
        for (int i=0; i<3; i++)
        {
            auto critTask = new CriteriaTask (data, index);
            Fitting::Method method;
            if (i == 0) method = Fitting::Kolmogorov;
            if (i == 1) method = Fitting::CramerVonMises;
            if (i == 2) method = Fitting::AndersonDarling;
            critTask->setCriteria(method);
            critTask->setHypothesis(hyp);
            tasks << critTask;
        }
        auto inrangeTask = new InrangeTask(data, index);

        inrangeTask->setLowerBoundary(src.getLowerBoundary());
        inrangeTask->setUpperBoundary(src.getUpperBoundary());
        tasks << inrangeTask;
    }

    int numSteps = tasks.size();

    QThreadPool::globalInstance()->setMaxThreadCount(threadCount);
    auto lambda_inrange = [&](AbstractTask* task)
    {
        emit sgnlInProcess(task->getIndex(), ResultsManager::PROB_INRANGE_SIGMA);
        emit sgnlInProcess(task->getIndex(), ResultsManager::PROB_INRANGE_BOTTOM);
        emit sgnlInProcess(task->getIndex(), ResultsManager::PROB_INRANGE_MEAN);
        task->run();
        step++;
        emit sgnlUpdateProgress(step*100/numSteps);

    };

    auto lambda_crit = [&](CriteriaTask* task)
    {
        ResultsManager::ResultType type {ResultsManager::CRAMER_MISES};
        switch (task->criteria())
        {
            case Fitting::Kolmogorov:
                type = ResultsManager::KOLMOGOROV;
                break;
            case Fitting::CramerVonMises:
                type = ResultsManager::CRAMER_MISES;
                break;
            case Fitting::AndersonDarling:
                type = ResultsManager::ANDERSON;
                break;
            default:
                break;
        }

        //emit sgnlInProcess(task.getIndex(), ResultsManager::DISTR_PARS);
        emit sgnlInProcess(task->getIndex(), type);
        task->run();
        step++;
        emit sgnlUpdateProgress(step*100/numSteps);
    };

    auto lambda = [&](AbstractTask* task)
    {
        auto critTask = dynamic_cast<CriteriaTask*>(task);
        if (critTask != nullptr)
            return lambda_crit(critTask);
        else
            return lambda_inrange(task);
    };

    future = QtConcurrent::map(tasks, lambda);
    future.waitForFinished();
    qDeleteAll(tasks);

    emit sgnlUpdateProgress(100);

    quit();

}


