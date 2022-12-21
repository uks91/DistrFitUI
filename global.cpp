#include <QVector>
#include <QMap>
#include <QVariant>
#include <QLocale>
#include <cmath>

template class QVector<QMap<QString, QVariant>>;
template class QVector<bool>;
template class QVector<double>;
template class QVector<int>;

QString numToString(double value, int prec, bool isPrecWithIntPart)
{
    auto extra_prec = isPrecWithIntPart ? 0 : static_cast<int> (ceil(log10(value)));
    const auto power = pow(10.0, prec);
    value = floor (value*power+0.5)/power;
    return QString::number(value, 'g', prec+extra_prec).replace('.', ',');
}
