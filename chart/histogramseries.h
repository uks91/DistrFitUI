#ifndef HISTOGRAMSERIES_H
#define HISTOGRAMSERIES_H

#include <QAreaSeries>
#include <QLineSeries>
#include <QChart>
#include <QChartView>
#include "bins.h"
#include "histogram.h"

extern template class QPair<real,real>;


QT_CHARTS_USE_NAMESPACE

//!
//! \brief The HistogramSeries class
//!

class HistogramSeries : public QAreaSeries
{
    Q_OBJECT

public:
    explicit HistogramSeries (QObject *parent = Q_NULLPTR);
    HistogramSeries(QLineSeries *upperSeries, QLineSeries *lowerSeries = Q_NULLPTR);
    ~HistogramSeries();


    void setBins (const Bins& mBins);
    void setData (const realVec &data);
    QRectF getRangeHint() const {return rangeHint;}
    bool isValidRange () {return hist.isInRange();}

signals:
    //void sgnlRangeHint(QRectF rectf);
    //void sgnlValidateRange (bool);
    void sgnlModified ();

private:
    QLineSeries *ser0;
//    QLineSeries ser1;

    Histogram hist;

    QRectF rangeHint;

    void rebuild ();

};


#endif // HISTOGRAMSERIES_H
