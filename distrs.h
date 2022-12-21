#ifndef DISTRS_H
#define DISTRS_H

#include <QStringList>

QList<QStringList> distributions;

distributions << (QStringList() << "1" << "N"  << "Нормальное");
distributions << (QStringList() << "2" << "LN" << "Логнормальное");
distributions << (QStringList() << "3" << "W"  << "Вейбулла");

#endif // DISTRS_H
