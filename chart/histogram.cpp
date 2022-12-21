#include "histogram.h"
#include <algorithm>
#include <numeric>

Histogram::Histogram(const Bins &bins, const realVec &data)
    : mBins      {bins}
    , mData      {data}
    , hist       {realVec (mBins.getN(), 0.0)}
    , form       {new __HistogramForms::PdfForm()}
    , outOfRange {false}
{
    rebuild();
    std::sort (mData.begin(), mData.end());
}

Histogram::~Histogram()
{
    delete form;
}

real Histogram::at(int index) const
{
    if (index < 0 || index >= size())
        return 0;
    return hist.at(index);
}

QRectF Histogram::border() const
{
    QRectF out;
    if (!mBins.isValid())
        return out;
    out.setLeft(mBins.getXmin());
    out.setRight(mBins.getXmax());
    out.setBottom(0.0);
    //! BUG: return NAN in some cases!
    real max = *std::max_element(hist.cbegin(), hist.cend());
    out.setTop(max);
    return out;
}

QRectF Histogram::rectAt(int index) const
{
    QRectF out {};
    if (index < 0 || index >= size())
        return out;
    out.setBottom(0.0);
    out.setTop(hist.at(index));
    auto binborder = mBins.getBorderByIndex(index);
    out.setLeft(binborder.first);
    out.setRight(binborder.second);
    return out;
}

void Histogram::setBins(const Bins &bins)
{
    if (mBins == bins)
        return;
    mBins = bins;
    rebuild();
}

void Histogram::setData(const realVec &data)
{
    if (mData == data)
        return;
    mData = data;
    std::sort (mData.begin(), mData.end());
    rebuild();
}

void Histogram::rebuild()
{
    if (!mBins.isValid() || mData.empty())
    {
        hist = realVec (0);
        return;
    }

    outOfRange = false;
    IntVec tmp (mBins.getN(), 0);

    for (const auto& elem : mData)
    {
        int idx = mBins.getBinIndex(elem);
        if (idx == -1)
        {
            outOfRange = true;
            continue;
        }
        tmp[idx] += 1.0;
    }
    hist = form->toForm(tmp, mBins);
    for (auto& val : hist) {
        if (val != val)
            val = 0;
    }
}

#ifdef QDEBUG_H
QDebug operator<<(QDebug deb, const Histogram &histogram)
{
    QStringList list;

    for (int i=0; i<histogram.size(); i++)
    {
        list << QString("%1 -> %2").arg(i).arg(histogram.at(i));
    }

    deb << QString ("Histogram {%1}\n").arg(list.join(", ")).toStdString().data();

    return deb;
}
#endif

realVec __HistogramForms::PdfForm::toForm(const IntVec &counts, const Bins &bins)
{
    int sum = std::accumulate(counts.cbegin(), counts.cend(), 0);
    auto out = realVec (counts.size(), 0.0);

    std::transform (counts.cbegin(), counts.cend(), out.begin(), [&](const int value)
    {
        return value/(bins.getDx()*sum);
    });

    return out;
}
