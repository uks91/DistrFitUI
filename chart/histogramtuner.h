#ifndef HISTOGRAMTUNER_H
#define HISTOGRAMTUNER_H

#include <QWidget>
#include "global.h"
#include "bins.h"

extern template class QPair<real,real>;
extern template class QVector<QMap<QString, QVariant>>;
extern template class QVector<bool>;
extern template class QVector<double>;
extern template class QVector<int>;

namespace Ui {
class HistogramTuner;
}

class HistogramSeries;

class HistogramTuner : public QWidget
{
    Q_OBJECT

public:
    enum class Target {HistBins, AreaColor, LineColor, LineWidth};
    Q_ENUM (Target)
    explicit HistogramTuner(QWidget *parent = 0);
    ~HistogramTuner();

    Bins   getBins      () const;
    QRectF getHistogramRange () const;

public slots:
    void slotRadButtonChanged(bool b);
    void slotHistBinsChanged();
    void slotValidateRange (bool value);
    void slotFitHistogramParameters();

private:
    void lock(bool b);

    Ui::HistogramTuner *ui;
    Bins bins;
public:
    HistogramSeries* histogram;
};

#endif // HISTOGRAMTUNER_H
