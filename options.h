#ifndef OPTIONS_H
#define OPTIONS_H

#include <QObject>
#include <QList>

class Options : public QObject
{
    Q_OBJECT
public:
    enum {LATIN, NAME};
    explicit Options(QObject *parent = nullptr);

    static const QList<QStringList> &getDistributions();
    static const QList<QStringList> &getMethods();


private:
    static QList<QStringList> distributions;    // Общая информация о распределениях
    static QList<QStringList> methods;    // Общая информация о распределениях

};

#endif // OPTIONS_H
