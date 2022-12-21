#ifndef GLOBAL_H
#define GLOBAL_H

#include <QVector>
#include <QMap>
#include <QVariant>
#include <QtDebug>
#include <memory>

using real     = double;
using VarTable = QVector<QMap<QString, QVariant>>; //TODO: exclude from global.h
using BoolVec  = QVector<bool>;
using realVec  = QVector<real>;
using IntVec   = QVector<int>;
enum CalcOption {Criteria, Distribution, Method};
//using array1 = std::vector<qreal>;
//using array2 = std::vector<array1>;

namespace Const {
const int     countCriteria      = 3;
const int     countDistrs        = 7;
const int     countMethods       = 3;

//const real    chartMargin

const QChar   srcDataSplitter    = ';';
const QString datasetDefaultName = "DatasetDefaultName";
const QString dateFormat         = "yyyy-MM-dd";

constexpr static real min_limit = std::numeric_limits<double>::lowest();
}


QString numToString (double value, int prec=3, bool isPrecWithIntPart = false);

extern template class QVector<QMap<QString, QVariant>>;
extern template class QVector<bool>;
extern template class QVector<double>;
extern template class QVector<int>;

#endif // GLOBAL_H
