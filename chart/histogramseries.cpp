#include "histogramseries.h"

HistogramSeries::HistogramSeries(QObject *parent)
    : QAreaSeries(parent)
    , ser0{new QLineSeries()}
    , hist{Histogram()}
{
    setUpperSeries(ser0);
}

HistogramSeries::HistogramSeries(QLineSeries *upperSeries, QLineSeries *lowerSeries)
    : QAreaSeries (upperSeries, lowerSeries)
    , ser0{new QLineSeries()}
    , hist {Histogram()}
{
}

HistogramSeries::~HistogramSeries()
{
    delete ser0;
}


void HistogramSeries::setBins(const Bins &bins)
{
    hist.setBins(bins);
    rebuild();
}

void HistogramSeries::setData(const realVec &data)
{
    hist.setData(data); // change bins too!
    rebuild();
}

void HistogramSeries::rebuild()
{
    // Перестраиваем гистограмму
    ser0->clear();

    for (int i=0; i<hist.size(); i++)
    {
        auto border = hist.rectAt(i);
        if (i==0)
            ser0->append(border.bottomLeft());
        ser0->append(border.topLeft());
        ser0->append(border.topRight());
        ser0->append(border.bottomRight());
    }

    QRectF rectf = hist.border();
    real widthMargin = 0.01*rectf.width();
    real topMargin = 0.02*rectf.height();
    QMarginsF margins {widthMargin, topMargin, widthMargin, 0.0};
    rectf += margins;
    rangeHint = rectf;

    emit sgnlModified();
    return ;
    //emit sgnlValidateRange(hist.isInRange());
    //emit sgnlRangeHint(rectf);
}
