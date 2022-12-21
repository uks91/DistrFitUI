// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "task.h"
#include "managers/resultsmanager.h"
#include <distributions.h>
#include <QFile>
#include <numeric>
#include <random>

extern AbstractDistribution* chooseById(int id);


AbstractTask::AbstractTask(const AbstractTask &other)
    : AbstractTask (other.data, other.index)
{

}

AbstractTask::AbstractTask(const array1 &sourceData, const ResultsIndex &index):
    data  {sourceData},
    index {index}
{

}

ResultsIndex AbstractTask::getIndex() const
{
    return index;
}


AbstractTask &AbstractTask::operator=(const AbstractTask &other)
{
    *this = other;
    return *this;
}

Fitting::Estimation AbstractTask::estimationMethod()
{
    int j = index.getMethodId();

    if (j == 0)
        return Fitting::MD;
    else if (j == 1)
        return Fitting::MLH;
    else
        return Fitting::SIMPLE_MLH;
}

//!
//! \brief EstimationTask::EstimationTask
//! \param sourceData
//! \param index
//!

EstimationTask::EstimationTask(const EstimationTask &other)
    : AbstractTask (other)
{

}

EstimationTask::EstimationTask(const array1 &sourceData, const ResultsIndex &index)
    : AbstractTask(sourceData, index)
{

}

EstimationTask &EstimationTask::operator =(const EstimationTask &other)
{
    *this = other;
    return *this;
}


void EstimationTask::run()
{
    // TODO: commented for hiding errors
//    AbstractDistribution *distr = chooseById(index.getDistrId());
//    if (distr == nullptr)
//        return;

//    distr->setEstimationMethod(estimationMethod());

//    distr->setEmpData(data, true);
//    auto pars = distr->getPars();

//    auto& results = ResultsManager::instance();
//    results.setDistrPars(index, realVec::fromStdVector(pars));

//    delete distr;
}


//!
//! \brief CriteriaTask::CriteriaTask
//! \param other
//!

Fitting::Hypothesis CriteriaTask::hyp = Fitting::Simple;
int CriteriaTask::roundDigits = 0;
int CriteriaTask::numIters = 1E4;


CriteriaTask::CriteriaTask(const CriteriaTask &other)
    : AbstractTask(other)
    , crit {other.crit}
{

}

CriteriaTask::CriteriaTask(const array1 &sourceData, const ResultsIndex &index)
    : AbstractTask(sourceData, index)
{

}

CriteriaTask &CriteriaTask::operator =(const CriteriaTask &other)
{
    *this = other;
    return *this;
}

void CriteriaTask::setCriteria(Fitting::Method crit)
{
    this->crit = crit;
}

void CriteriaTask::setHypothesis(const Fitting::Hypothesis hyp)
{
    CriteriaTask::hyp = hyp;
}

void CriteriaTask::setRoundDigits(const int digits)
{
    roundDigits = digits;
}

void CriteriaTask::setNumIters(const int iters)
{
    numIters = iters;
}

void CriteriaTask::run()
{
    auto& res = ResultsManager::instance();
    SingleResult single = res.getResultByIndex(index);
    if (!single.isParsDefined)
    {
        EstimationTask estTask (data, index);
        estTask.run();
        single = res.getResultByIndex(index);
    }

    AbstractDistribution *distr = chooseById(index.getDistrId());
    if (distr == nullptr)
        return;
    auto critValue {0.0};
    auto pvalue {0.0};

    bool isValidPars = !std::any_of(single.pars.cbegin(), single.pars.cend(), [] (double value)
    {
        return std::isnan(value) || std::isinf(value);
    });

    if (isValidPars)
    {
        // TODO: commented for hiding errors
//        distr->setEstimationMethod(estimationMethod());
//        distr->setEmpData(data, false);
//        distr->setParameter(single.pars.toStdVector());

//        Fitting fit {data, distr, crit, hyp};
//        fit.setHighSpeed(false);
//        fit.setDigits(roundDigits);
//        fit.runTest(true, numIters);
//        critValue = fit.resultCriteria();
//        pvalue    = fit.resultPValue();
    }
    res.setFitResults(index, crit, critValue, pvalue);


    delete distr;
}

int InrangeTask::numIters {10};
real InrangeTask::tolerance {0.9};

InrangeTask::InrangeTask(const InrangeTask &other)
    : AbstractTask (other)
    , lower {other.lower}
    , upper {other.upper}
    , probInrange {other.probInrange}
{

}

InrangeTask::InrangeTask(const array1 &sourceData, const ResultsIndex &index)
    : AbstractTask(sourceData, index)
    , lower {Const::min_limit}
    , upper {Const::min_limit}
    , probInrange {0}
{

}

InrangeTask &InrangeTask::operator =(const InrangeTask &other)
{
    *this = other;
    return *this;
}

void InrangeTask::setRange(const real lower, const real upper)
{
    setLowerBoundary(lower);
    setUpperBoundary(upper);
}

void InrangeTask::setUpperBoundary(const real upper)
{
    this->upper = upper;
}

void InrangeTask::setLowerBoundary(const real lower)
{
    this->lower = lower;
}

void InrangeTask::setNumIters(const int iters)
{
    numIters = iters;
}

void InrangeTask::setTolerance(const real value)
{
    tolerance = value;
}

int uniform (int from, int till)
{
    static std::mt19937 engine (QDateTime::currentSecsSinceEpoch());
    auto distr = std::uniform_int_distribution<int> (from, till);
    return distr (engine);
//    return floor(distr (engine)+0.5);

//    static auto uniform_ = UniformDistr(from, till);
//    return floor(uniform_.rnd()+0.5);
}

void InrangeTask::run()
{
    AbstractDistribution *distr0 = chooseById(index.getDistrId());
    if (distr0 == nullptr)
        return;

    AbstractDistribution *distr1 = chooseById(index.getDistrId());

    auto& res = ResultsManager::instance();
    SingleResult single = res.getResultByIndex(index);
    if (!single.isParsDefined)
    {
        EstimationTask estTask (data, index);
        estTask.run();
        single = res.getResultByIndex(index);
    }

    bool isValidPars = !std::any_of(single.pars.cbegin(), single.pars.cend(), [] (double value)
    {
        return std::isnan(value) || std::isinf(value);
    });

    if (!isValidPars)
    {
        res.setProbInrange(index, 0);
        res.setProbInrangeMean  (index, 0);
        res.setProbInrangeSigma (index, 0);
        res.setProbInrangeBottom(index, 0);
        delete distr1;
        delete distr0;
        return;
    }

    realVec probabilities (numIters, 0);
// TODO: commented for hiding errors
//    distr0->setEstimationMethod(estimationMethod());
//    distr0->setParameter(single.pars.toStdVector());
    if (index.getDistrId() == 0)
    {
        auto pars = distr0->getPars();
        pars[1] *= 1;
        distr0->setParameter(pars);
    }
    // TODO: commented for hiding errors
//    distr1->setEstimationMethod(estimationMethod());

    auto assessInrangeProbability = [&] (AbstractDistribution *distr)
    {
        if (upper == Const::min_limit && lower == Const::min_limit)
            return 0.0;
        else
        {
            real pUpper = upper==Const::min_limit ? 1.0 : distr->cdf(upper);
            real pLower = lower==Const::min_limit ? 0.0 : distr->cdf(lower);
            return pUpper - pLower;
        }
    };

    probInrange = assessInrangeProbability(distr0);
    //    qDebug () << probInrange;
    res.setProbInrange(index, probInrange);

//    auto bootstrap = [&] (const array1& src)
//    {
//        auto N = src.size();

//        //static auto uniform_ = UniformDistr(0.0, N-1);
//        auto out = array1 (N);
//        QVector <real> idxs;


//        std::generate (out.begin(), out.end(), [&] {
//            auto dbl = uniform(0.0, N-1);
//            int idx = floor(dbl+0.5/*uniform_.rnd()+0.5*/);
//            //idx = uniform(0.0, N-1.0);
//            idxs.append(dbl);
//            return src[idx];
//        });
//        //qDebug () << idxs;
//        return out;
//    };

    const int size = data.size();
    //Fitting fit0 (data, distr0);
    //auto S0 = fit0.criteriaValue();
    std::for_each (probabilities.begin(), probabilities.end(), [&] (double &prob)
    {
        //auto S1 {S0};
        /*bool b {false};
        do
        {
            auto x = distr0->generateSeries(size);
            distr1->setEmpData(x, true);
            //fit0.setData(x);
            //S1 = fit0.criteriaValue();
//            break;
            b = std::all_of (x.cbegin(), x.cend(), [&] (const real x)
            {
                return x > this->lower && x < this->upper;
            });
            if (!b)
                ++countOfRange;

        }
        while (!b);*/

        auto x = distr0->generateSeries(size);
        // TODO: commented for hiding errors
//        distr1->setEmpData(x, true);


        //auto x2 = distr1->generateSeries(size);
        //distr1->setEmpData(x2, true);
//        auto x = bootstrap (data);
//        x = bootstrap(x);
        //distr1->setEmpData(x, true);
        prob = assessInrangeProbability(distr1);
    });
    //qDebug () << countOfRange;

    // Removing incorrect values less than 0 or higher than 1
    probabilities.erase(std::remove_if(probabilities.begin(), probabilities.end(), [](double val)
    {
        return val < 0.0 || val > 1.0;
    }), probabilities.end());


    std::sort (probabilities.begin(), probabilities.end());
    auto filename = QString("inrange/%1_%2.dat").arg(index.getDistrName()).arg(index.getMethodName());
    QFile file (filename);
    file.open(QFile::WriteOnly);
    QTextStream out (&file);
    for (const auto& val : probabilities)
    {
        out << QString::number(val, 'g', 10) << endl;
    }
    auto N = static_cast<size_t> ((1.0-tolerance)*probabilities.size());

    auto bottomProb = 0.0;
    try

    {
        using namespace std;
        auto size = static_cast<size_t>(probabilities.size());
        auto mean = accumulate  (probabilities.cbegin(), probabilities.cend(), 0.0)/size;
        auto sigma = accumulate (probabilities.cbegin(), probabilities.cend(), 0.0, [mean, size] (double result, double value)
        {
            result += pow(value-mean, 2.0)/size;
            return result;
        });
        //qDebug () << "Sigma: " << sigma;

        res.setProbInrangeMean  (index, mean);
        res.setProbInrangeSigma (index, sqrt(sigma));
        //! res.setProbInrangeBottom(index, emp.quantile(1.0-tolerance)); // BUG: it throws exception, especially when prob-ies is {1,1,....}
        //! WARNING: It's a patch!
        if (probabilities.size() > 0)
            bottomProb = N >= size ? probabilities[size-1] : probabilities[N];

        else
        {
            bottomProb = 0.0;
        }
        res.setProbInrangeBottom(index, bottomProb);
    }
    catch (...)
    {

    }

    delete distr0;
    delete distr1;
}
