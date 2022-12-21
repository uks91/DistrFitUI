// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "bins.h"
#include <cmath>


Bins::Bins()
    : Bins (0,0,0)
{

}

Bins::Bins(const Bins &other)
//    : HistogramPockets(other.x0, other.step, other.count)
{
    *this = other;
}

Bins Bins::generateByVarDx(const real xmin, const real xmax, const int N)
{
    return Bins (xmin, (xmax-xmin)/N, N);
}

Bins Bins::generateByVarN(const real xmin, const real xmax, const real dx)
{
    double n = (xmax-xmin)/dx;
    auto N = static_cast<int> (floor(n+0.5));
    return Bins (xmin, dx, N);
}

Bins Bins::generateByVarXmin(const real xmax, const real dx, const int N)
{
    return Bins (xmax-N*dx, dx, N);
}

Bins Bins::generateByVarXmax(const real xmin, const real dx, const int N)
{
    return Bins (xmin, dx, N);
}

int Bins::getBinIndex(real value) const
{
    if (value < x0 || value > getXmax())
        return -1;
    auto idx = static_cast<int> (floor((value-x0)/step));
    idx = idx < count ? idx : count-1;
    return idx;
}

BinBorder Bins::getBorderByIndex(int index) const
{
    if (index < 0 || index >= count)
        return BinBorder();
    auto xmin = x0 + step*index;
    auto xmax = x0 + step*(index+1);
    return qMakePair(xmin, xmax);
}

Bins &Bins::operator =(const Bins &other)
{
    this->x0    = other.x0;
    this->step  = other.step;
    this->count = other.count;
    this->valid = other.valid;
    return *this;
}

bool Bins::operator ==(const Bins &other)
{
    return    this->x0    == other.x0
           && this->step  == other.step
           && this->count == other.count;
}

bool Bins::operator !=(const Bins &other)
{
    return !(*this == other);
}

Bins::Bins(const real xmin, const real dx, const int N)
    : x0    {xmin}
    , step  {dx}
    , count {N}
    , valid {checkValidity(step, count)}
{

}

bool Bins::checkValidity(const real dx, const int N)
{
    return (dx > 0) && N > 1;
}

bool operator ==(const Bins &first, const Bins &second)
{
    return first==second;
}

bool operator !=(const Bins &first, const Bins &second)
{
    return first != second;
}

#ifdef QDEBUG_H
QDebug operator<<(QDebug deb, const Bins &bins)
{
    deb << QString ("Bins {%1, %2, %3, (%4)}\n")
           .arg(bins.getXmin())
           .arg(bins.getDx())
           .arg(bins.getN())
           .arg(bins.getXmax())
           .toStdString().data();

    return deb;
}
#endif
