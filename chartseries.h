#ifndef CHARTSERIES_H
#define CHARTSERIES_H

#include <QLineSeries>
#include <memory>

QT_CHARTS_USE_NAMESPACE

class AbstractDistribution;

//!
//!
//! \brief The PDFSeries class
//!
//!

class PDFSeries : public QLineSeries
{
    Q_OBJECT
public:
    explicit PDFSeries (QObject *parent = nullptr);

    void setVisibleRange    (const QRectF &rect);
    QRectF getRangeHint () const;
    void setDistr (const std::shared_ptr<AbstractDistribution>& distr);
    void setDistr (int distrId, const QVector<double>& parameters);

private:
    QRectF range;
    std::shared_ptr<AbstractDistribution> distr;

    void rebuild ();
};


#endif // CHARTSERIES_H
