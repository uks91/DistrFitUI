// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "chartseries.h"
#include <QtDebug>
#include "distributions.h"


////!
////!
////! \brief HistogramSeries::HistogramSeries
////!
////!

extern AbstractDistribution *chooseById(int id);

using std::shared_ptr;

QPointF findMode (const shared_ptr<AbstractDistribution> &distr)
{
    QPointF out;
    if (distr == nullptr)
        return out;

//    double mean = distr->mean(); // TODO: commented for hiding errors
    double mean = 0;
//    double stdev = sqrt(distr->variance()); // TODO: commented for hiding errors
    double stdev = 0;

    double xmin = mean-6*stdev;
    double xmax = mean+6*stdev;
    double dx = (12*stdev)/50.0;

    for (double x = xmin; x <= xmax; x += dx)
    {
        double pdf = distr->pdf(x);
        if (pdf > out.y())
        {
            out.setX(x);
            out.setY(pdf*1.05);
        }
    }
    return out;
}

QRectF findBorders (const shared_ptr<AbstractDistribution> &distr)
{
    QRectF out;
    if (distr == nullptr)
        return  out;

    QPointF mode = findMode(distr);
//    double stdev = sqrt(distr->variance()); // TODO: commented for hiding errors
    double stdev = 0;
    double left;
    double right;

    if (mode.isNull())
        return out;
    double dx = (12*stdev)/50.0;

    // finding left border
    for (double x = mode.x()-6*stdev; x <= mode.x(); x += dx)
    {
        double pdf = distr->pdf(x);
        if (Q_UNLIKELY(pdf > mode.y()*0.02))
        {
            break;
        }
        left = x;
    }

    // finding right border
    for (double x = mode.x()+6*stdev; x >= mode.x(); x -= dx)
    {
        double pdf = distr->pdf(x);
        if (Q_UNLIKELY(pdf > mode.y()*0.02))
        {
            break;
        }
        right = x;
    }

    out.setTopLeft(QPointF(left, mode.y()));
    out.setBottomRight(QPointF(right, 0.0));
    return out;
}


PDFSeries::PDFSeries(QObject *parent)
    : QLineSeries(parent)
    , distr {nullptr}
{
}

void PDFSeries::setVisibleRange(const QRectF &rect)
{
    range = rect;
    rebuild ();
}

QRectF PDFSeries::getRangeHint() const
{
    // возвращает прямоугльник с 5 и 95 % квантилью слева-справа и максимальное значение сверху
    return findBorders(this->distr);
}

void PDFSeries::setDistr(const shared_ptr<AbstractDistribution> &distr)
{
    this->distr = distr;
    rebuild();
}

void PDFSeries::setDistr(int distrId, const QVector<double> &parameters)
{
    this->distr = shared_ptr<AbstractDistribution>(chooseById(distrId));
    if (this->distr == nullptr)
        return;

    // TODO: commented for hiding errors
//    this->distr->setParameter(parameters.toStdVector());
    rebuild();
}

void PDFSeries::rebuild()
{
    if (range.right() <= range.left() || distr == nullptr)
        return;

    double dx = (range.right()-range.left())/500.0;
    QList<QPointF> points;
    for (double x = range.left(); x<=range.right()+dx; x += dx)
    {
        double pdf = distr->pdf(x);
        if (pdf != pdf) {continue;}
        points << QPointF(x, pdf);
    }
    clear();
    append(points);
}

