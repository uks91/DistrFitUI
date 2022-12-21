// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "options.h"

QList<QStringList> Options::distributions = QList<QStringList> ()
        << (QStringList() /*<< "1"*/ << "Normal"       << "Нормальное")
        << (QStringList() /*<< "2"*/ << "LogNormal"    << "Логнормальное")
        << (QStringList() /*<< "3"*/ << "Weibull"      << "Вейбулла")
        << (QStringList() /*<< "3"*/ << "Logistic"     << "Логистическое")
        << (QStringList() /*<< "3"*/ << "Moyal"        << "Мойала")
        << (QStringList() /*<< "3"*/ << "ExtremeValue" << "Максимального значения")
        << (QStringList() /*<< "3"*/ << "Gumbel"       << "Минимального значения");

QList<QStringList> Options::methods = QList<QStringList> ()
        << (QStringList() << "MD"         << "Минимального расстояния")
        << (QStringList() << "MLH"        << "Максимального правдоподобия")
        << (QStringList() << "SIMPLE_MLH" << "Максимального правдоподобия (простой)");


Options::Options(QObject *parent) : QObject(parent)
{

}

const QList<QStringList>& Options::getDistributions()
{
    return distributions;
}

const QList<QStringList>& Options::getMethods()
{
    return methods;
}
