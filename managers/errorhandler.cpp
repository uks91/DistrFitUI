#include "errorhandler.h"
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDateTime>

class ErrorHandler::ErrorHandlerImpl
{
public:
    explicit ErrorHandlerImpl ();

    bool switchFileLogging(bool isOn);
    void criticalMessage(const QString &message);
    void infoMessage(const QString &message);
    void wariningMessage(const QString &message);

private:
    bool        fileLogging;
    bool        fileLoggingAvailability;
    QFile       file;
    QTextStream out;

    void printMessageToFile (const QString &messageType, const QString &message);
    const QString logDirName {"logs"};
};

ErrorHandler::ErrorHandlerImpl::ErrorHandlerImpl()
    : fileLogging {true}
    , fileLoggingAvailability {true}
    , file {}
    , out  {&file}
{
    QDir logDir {logDirName};
    if (!logDir.exists())
    {
        fileLoggingAvailability = QDir().mkdir(logDirName);
    }

    QDateTime currentDateTime {QDateTime::currentDateTime()};
    file.setFileName(QString("%1/log_%2.log").arg(logDirName).arg(currentDateTime.toString("yyyy-MM-dd_hh-mm")));
    fileLoggingAvailability = file.open(QFile::WriteOnly);
    fileLogging = fileLogging && fileLoggingAvailability;
}

bool ErrorHandler::ErrorHandlerImpl::switchFileLogging(bool isOn)
{
    if (!fileLoggingAvailability)
        return false;

    bool tmp {fileLogging};
    fileLogging = isOn;
    return tmp;
}

void ErrorHandler::ErrorHandlerImpl::criticalMessage(const QString &message)
{
    printMessageToFile("critical", message);
}

void ErrorHandler::ErrorHandlerImpl::infoMessage(const QString &message)
{
    printMessageToFile("info", message);
}

void ErrorHandler::ErrorHandlerImpl::wariningMessage(const QString &message)
{
    printMessageToFile("warning", message);
}

void ErrorHandler::ErrorHandlerImpl::printMessageToFile(const QString &messageType, const QString &message)
{

    out << QString("[%1] %2 - %3")
           .arg(messageType)
           .arg(QDateTime::currentDateTime().toString("hh:mm:ss:zzz"))
           .arg(message)
        << endl;
}



//!
//! \brief ErrorHandler::ErrorHandler
//! \param parent
//!

ErrorHandler::ErrorHandler(QObject *parent)
    : QObject(parent)
    //, impl {std::make_shared<ErrorHandlerImpl>()}
{

}

ErrorHandler &ErrorHandler::instance()
{
    static ErrorHandler singleton;
    return singleton;
}

bool ErrorHandler::switchFileLogging(bool isOn)
{
    return impl->switchFileLogging(isOn);
}

void ErrorHandler::criticalMessage(const QString &message)
{
    impl->criticalMessage(message);
    emit sgnlCritical(message);
}

void ErrorHandler::infoMessage(const QString &message)
{
    impl->infoMessage(message);
    emit sgnlInfo(message);
}

void ErrorHandler::wariningMessage(const QString &message)
{
    impl->wariningMessage(message);
    emit sgnlWarning(message);
}


