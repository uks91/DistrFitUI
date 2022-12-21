#ifndef BINS_H
#define BINS_H

#include "../global.h"
#include <QPair>
using BinBorder = QPair<real,real>;

class Bins
{
public:
    Bins();
    Bins(const Bins& other);

    static Bins generateByVarDx   (const real xmin, const real xmax, const int N);
    static Bins generateByVarN    (const real xmin, const real xmax, const real dx);
    static Bins generateByVarXmin (const real xmax, const real dx,   const int N);
    static Bins generateByVarXmax (const real xmin, const real dx,   const int N);

    int getBinIndex (real value) const;
    BinBorder getBorderByIndex (int index) const;

    real  getDx   () const {return step;}
    int   getN    () const {return count;}
    real  getXmax () const {return x0+step*count;}
    real  getXmin () const {return x0;}

    bool isValid () const {return valid;}

    Bins& operator = (const Bins& other);
    bool  operator ==(const Bins& other);
    bool  operator !=(const Bins& other);

private:
    real  x0;
    real  step;
    int   count;
    bool  valid;

    Bins (const real xmin, const real dx, const int N);
    static bool checkValidity (const real dx, const int N);
};

bool operator ==(const Bins& first, const Bins& second);
bool operator !=(const Bins& first, const Bins& second);

#ifdef QDEBUG_H
QDebug operator<<(QDebug deb, const Bins &bins);
#endif

#endif // BINS_H
