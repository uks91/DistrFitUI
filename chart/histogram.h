#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "bins.h"
#include "../global.h"
#include <QRectF>

namespace __HistogramForms {

struct AbstractForm
{
    virtual ~AbstractForm() = default;
    virtual realVec toForm (const IntVec& counts, const Bins& bins) = 0;
};

struct PdfForm : public AbstractForm
{
    realVec toForm(const IntVec& counts, const Bins& bins) override;
};

}


class Histogram
{
public:
    Histogram (const Bins &bins=Bins(), const realVec& data=realVec());
    ~Histogram();

    real at (int index) const;
    QRectF border () const;
    bool isInRange () const {return !outOfRange;}
    QRectF rectAt (int index) const;
    inline const Bins& getBins () const {return mBins;}

    void setBins (const Bins &bins);
    void setData (const realVec &data);

    int size () const {return hist.size();}

private:
    Bins    mBins;
    realVec mData;
    realVec hist;
    __HistogramForms::AbstractForm* form;
    bool outOfRange;

    void rebuild();

};

#ifdef QDEBUG_H
QDebug operator<<(QDebug deb, const Histogram& res);
#endif

#endif // HISTOGRAM_H
