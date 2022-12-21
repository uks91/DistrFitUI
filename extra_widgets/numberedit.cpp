// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "numberedit.h"
#include <QDoubleValidator>
#include <QKeyEvent>
#include <QtDebug>
//#include <cmath>
#include <QLocale>
#include <QHBoxLayout>

#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/math/special_functions.hpp>
#include <iostream>
#include <exception>
//!
//! \brief NumberEdit::NumberEdit
//!

namespace __private {

using decimal = boost::multiprecision::cpp_dec_float_50;

class NumberEditPrivate::NumberEditPrivateImpl
{

public:
    QString step;
    /*decimal min;
    decimal max;*/
    NumberEditPrivateImpl (const QString& step)
        : step{step}
        /*, min {std::numeric_limits<decimal>::min()}
        , max {std::numeric_limits<decimal>::max()}*/
    {}

    /*void setRange (const decimal& min, const decimal &max)
    {
        if (max >= min)
            return;
        this->min = min;
        this->max = max;
    }

    void setRange (const QString& min, const QString &max)
    {
        decimal dmin = decimal(min.toStdString());
        decimal dmax = decimal(max.toStdString());
        if (dmax >= dmin)
            return;
        this->min = dmin;
        this->max = dmax;
    }*/

};

NumberEditPrivate::NumberEditPrivate (QWidget *parent)
    : QLineEdit(parent)
    , impl {std::make_unique<NumberEditPrivateImpl>("-1")}
{
}

NumberEditPrivate::NumberEditPrivate(const QString &contents, QWidget *parent)
    : QLineEdit(contents, parent)
    , impl {std::make_unique<NumberEditPrivateImpl>("-1")}
{
}

void NumberEditPrivate::setStep(const QString &step)
{
    bool ok;
    QString step2 = step;
    step2.replace(".", ",");
    locale().toDouble(step2, &ok);
    if (ok)
        impl->step = step2;
}

template <typename T>
void NumberEditPrivate::setValue(T value)
{
    setText(locale().toString(value));
}

template <typename T>
T NumberEditPrivate::value() const
{
    double value = locale().toDouble(text());
    return static_cast<T>(value);
}

void NumberEditPrivate::change(int dir)
{
    using boost::multiprecision::fabs;
    using boost::multiprecision::pow;
    using boost::multiprecision::log10;
    using boost::multiprecision::floor;

    decimal step = decimal(QString(impl->step).replace(",", ".").toStdString());
    decimal x1;

    const QIntValidator* _validator = qobject_cast<const QIntValidator*>(validator());
    if (_validator != nullptr)
    {
        int value = locale().toInt(text());
        int delta = step > 0 ? dir*static_cast<int>(step) : dir;
        setText(locale().toString(value+delta));
        return;
    }

    decimal x0 = decimal (QString(text()).replace(",", ".").toStdString());
    if (step < 0)
    {
        decimal x0abs = fabs(x0);
        decimal p = floor(log10(x0abs));
        if (p < -10)
            p = -10;
        decimal p10 = pow(10, p);
        step = floor(x0abs/p10)*p10/100;
    }
    if (step == 0)
        step = decimal(1)/10;

    x1 = x0 + dir*step;
    setText(QString::fromStdString(x1.str()).replace(".", ","));
}

void NumberEditPrivate::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Down)
        change (-1);
    else if (event->key() == Qt::Key_Up)
        change (1);
    else if (event->key() == Qt::Key_Period)
    {
        QKeyEvent comma = QKeyEvent(event->type(),
                                    Qt::Key_Comma,
                                    event->modifiers(),
                                    ",",
                                    event->isAutoRepeat(),
                                    event->count());
        QLineEdit::keyPressEvent(&comma);
    }
    else
    {
        QLineEdit::keyPressEvent(event);
    }
}

void NumberEditPrivate::wheelEvent(QWheelEvent *event)
{
    int dir = event->angleDelta().y();
    change (dir/abs(dir));
}

DoubleValidator::DoubleValidator(QObject *parent)
    : QDoubleValidator(parent)
{}

DoubleValidator::DoubleValidator(int minimum, int maximum, int decimals, QObject *parent)
    : QDoubleValidator(minimum, maximum, decimals, parent)
{}

QValidator::State DoubleValidator::validate(QString &input, int &pos) const
{
    QValidator::State state = QDoubleValidator::validate(input, pos);
    bool ok;
    double value = locale().toDouble(input, &ok);
    if (!ok)
        return  QValidator::Invalid;

    if (value > this->top() || value < this->bottom())
        state = QValidator::Intermediate;
    else
        return QValidator::Acceptable;
    return state;
}

void DoubleValidator::fixup(QString &input) const
{
    QDoubleValidator::fixup(input);
    double value = locale().toDouble(input);
    if (value < bottom())
        input = locale().toString(bottom());
    if (value > top())
        input = locale().toString(top());
}

IntValidator::IntValidator(QObject *parent)
    : QIntValidator(parent)
{}

IntValidator::IntValidator(int minimum, int maximum, QObject *parent)
    : QIntValidator(minimum, maximum, parent)
{}

QValidator::State IntValidator::validate(QString &input, int &pos) const
{
    QValidator::State state = QIntValidator::validate(input, pos);
    int value = input.toInt();
    if (value > this->top() || value < this->bottom())
        state = QValidator::Intermediate;
    return state;
}

void IntValidator::fixup(QString &input) const
{
    QIntValidator::fixup(input);
    int value = input.toInt();
    if (value < bottom())
        input = locale().toString(bottom());
    if (value > top())
        input = locale().toString(top());
}

}

// END OF NAMESPACE __private


class NumberEdit::NumberEditUi
{
public:
    void setupUi(QWidget* wgt)
    {
        QHBoxLayout *layout = new QHBoxLayout(wgt);
        edt = new __private::NumberEditPrivate(wgt);
        QLocale loc = QLocale(QLocale::Russian, QLocale::Russia);
        loc.setNumberOptions(QLocale::OmitGroupSeparator);
        edt->setLocale(loc);
        layout->addWidget(edt);
        layout->setMargin(0);
    }
    __private::NumberEditPrivate* edt;
};

NumberEdit::NumberEdit(QWidget *parent)
    : QWidget(parent)
    , ui {new NumberEditUi}
{
    ui->setupUi(this);

}

NumberEdit::~NumberEdit()
{
    delete ui;
}

void NumberEdit::setStep(const QString &step)
{
    ui->edt->setStep(step);
}

void NumberEdit::setFrame(bool value)
{
    ui->edt->setFrame(value);
}

void NumberEdit::selectAll()
{
    ui->edt->selectAll();
}

DoubleNumberEdit::DoubleNumberEdit(QWidget *parent)
    : NumberEdit(parent)
{
    using namespace __private;
    val = new DoubleValidator(ui->edt);
    val->setDecimals(4);
    val->setLocale(this->locale());
    ui->edt->setValidator(val);
    connect (ui->edt, &NumberEditPrivate::textChanged, [this](const QString& str)
    {
        auto str2 = str;
        int i;
        auto state = val->validate (str2, i);
        if (state == QValidator::Intermediate)
        {
            val->fixup(str2);
            this->ui->edt->blockSignals(true);
            this->ui->edt->setText(str2);
            this->ui->edt->blockSignals(false);
        }
        else if (state == QValidator::Invalid)
        {
            return;
        }

        emit valueChanged(locale().toDouble(str));
    });
    auto i = ui->edt->impl;
}

void DoubleNumberEdit::setValue(double value)
{
    ui->edt->setValue(value);
}

double DoubleNumberEdit::value() const
{
    return ui->edt->value<double>();
}

void DoubleNumberEdit::setRange(double min, double max)
{
    val->setRange (min, max);
}

void DoubleNumberEdit::setMaximumRange(double max)
{
    setRange(std::numeric_limits<double>::min(), max);
}

void DoubleNumberEdit::setMinimumRange(double min)
{
    setRange(min, std::numeric_limits<double>::max());
}



IntNumberEdit::IntNumberEdit(QWidget *parent)
    : NumberEdit(parent)
{
    using namespace __private;
    val = new IntValidator(ui->edt);
    ui->edt->setValidator(val);
    ui->edt->setStep("1");

    connect (ui->edt, &NumberEditPrivate::textChanged, [this](const QString& str)
    {
        auto str2 = str;
        str2.replace(",", ".");
        int i;
        auto state = val->validate (str2, i);
        if (state == QValidator::Intermediate)
        {
            val->fixup(str2);
            this->ui->edt->blockSignals(true);
            this->ui->edt->setText(str2);
            this->ui->edt->blockSignals(false);
        }
        else if (state == QValidator::Invalid)
        {
            return;
        }

        emit valueChanged(str2.toInt());
    });
}

void IntNumberEdit::setValue(int value)
{
    ui->edt->setValue(value);
}

int IntNumberEdit::value() const
{
    return ui->edt->value<int>();
}

void IntNumberEdit::setRange(int min, int max)
{
    using namespace __private;
    val->setRange (min, max);
    /*ui->edt->impl->setRange(QString::number(min),
                            QString::number(max));*/
}

void IntNumberEdit::setMaximumRange(int max)
{
    setRange(std::numeric_limits<int>::min(), max);
}

void IntNumberEdit::setMinimumRange(int min)
{
    setRange(min, std::numeric_limits<int>::max());
}

