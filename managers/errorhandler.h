#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <QObject>
#include "../global.h"

class ErrorHandler : public QObject
{
    Q_OBJECT
private:
    ErrorHandler(QObject *parent = nullptr);
public:
    static ErrorHandler& instance ();
    bool switchFileLogging (bool isOn=true);

    void criticalMessage (const QString &message);
    void infoMessage     (const QString &message);
    void wariningMessage (const QString &message);

signals:
    void sgnlCritical (const QString &message);
    void sgnlInfo     (const QString &message);
    void sgnlWarning  (const QString &message);

public slots:

private:
    class ErrorHandlerImpl;
    std::shared_ptr<ErrorHandlerImpl> impl;

};

#endif // ERRORHANDLER_H
