#ifndef TASK_H
#define TASK_H

#include "global.h"
#include <fitting.h>
#include <managers/resultsmanager.h>
#include <functions.h>
//class ResultsIndex;

class AbstractTask
{
public:
    AbstractTask (const AbstractTask& other);
    AbstractTask (const array1 &sourceData, const ResultsIndex& index);
    virtual ~AbstractTask() = default;
    ResultsIndex getIndex () const;

    virtual void run () = 0;
    AbstractTask& operator= (const AbstractTask& other);

protected:
    const array1 data;
    const ResultsIndex index;


    Fitting::Estimation estimationMethod();
};

class EstimationTask : public AbstractTask
{
public:
    EstimationTask (const EstimationTask& other);
    EstimationTask (const array1 &sourceData, const ResultsIndex& index);
    EstimationTask &operator =(const EstimationTask &other);

    void run() override;
};

class CriteriaTask : public AbstractTask
{
public:
    CriteriaTask (const CriteriaTask& other);
    CriteriaTask (const array1 &sourceData, const ResultsIndex& index);
    CriteriaTask& operator =(const CriteriaTask& other);

    void setCriteria (Fitting::Method crit);
    Fitting::Method criteria () {return crit;}
    static void setHypothesis(const Fitting::Hypothesis hyp);
    static void setRoundDigits(const int digits);
    static void setNumIters   (const int iters);

    void run() override;
private:
    Fitting::Method crit;
    static Fitting::Hypothesis hyp;
    static int roundDigits;
    static int numIters;
};

class InrangeTask: public AbstractTask
{
public:
    InrangeTask (const InrangeTask& other);
    InrangeTask (const array1 &sourceData, const ResultsIndex& index);
    InrangeTask &operator =(const InrangeTask &other);

    void setRange (const real lower, const real upper);
    void setUpperBoundary (const real upper);
    void setLowerBoundary (const real lower);
    static void setNumIters   (const int iters);
    static void setTolerance  (const real value);

    void run() override;

private:
    real lower;
    real upper;
    real probInrange;

    static int numIters;
    static real tolerance;
};

#endif // TASK_H
