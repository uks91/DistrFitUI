#ifndef NUMBEREDIT_H
#define NUMBEREDIT_H

#include <QLineEdit>
#include <memory>
#include <QIntValidator>
#include <QDoubleValidator>
//#include <qaccessible.h>

class QKeyEvent;
class QWheelEvent;


//!
//! \brief The NumberEdit class
//!

class NumberEdit;
class DoubleNumberEdit;
class IntNumberEdit;

namespace __private {

class NumberEditPrivate : public QLineEdit
{
    Q_OBJECT
public:
    explicit NumberEditPrivate(QWidget *parent = Q_NULLPTR);
    NumberEditPrivate (const QString &contents, QWidget *parent = Q_NULLPTR);

    template <typename T>
    void   setValue (T value);

    template <typename T>
    T value    () const;

    void setStep (const QString &step);
    friend class ::NumberEdit;
    friend class ::DoubleNumberEdit;
    friend class ::IntNumberEdit;

signals:
    void valueChanged(double);

protected:
     void change          (int dir);
     void keyPressEvent   (QKeyEvent *event);
     void wheelEvent      (QWheelEvent *event);

private:
     class NumberEditPrivateImpl;
     std::shared_ptr<NumberEditPrivateImpl> impl;

     int test() {return 1;}
//     QString _step;
};

class DoubleValidator : public QDoubleValidator
{
    Q_OBJECT
public:
    DoubleValidator(QObject *parent = nullptr);
    DoubleValidator(int minimum, int maximum, int decimals, QObject *parent = nullptr);
    QValidator::State validate(QString &input, int &pos) const override;
    void fixup(QString &input) const override;
    virtual ~DoubleValidator() {}
};

class IntValidator : public QIntValidator
{
    Q_OBJECT
public:
    IntValidator(QObject *parent = nullptr);
    IntValidator(int minimum, int maximum, QObject *parent = nullptr);
    QValidator::State validate(QString &input, int &pos) const override;
    void fixup(QString &input) const override;
    virtual ~IntValidator() {}
};

}

class NumberEdit : public QWidget
{
    Q_OBJECT

public:
    explicit NumberEdit(QWidget *parent = nullptr);
    virtual ~NumberEdit();
    void setStep (const QString &step);
    void setFrame (bool value);

protected:
    class NumberEditUi;
    //friend class __private::NumberEditPrivate;
    NumberEditUi *ui;

public slots:
    void selectAll ();
};

class DoubleNumberEdit : public NumberEdit
{
    Q_OBJECT

public:
    explicit DoubleNumberEdit(QWidget *parent = nullptr);
    //virtual ~NumberEdit();

    void setValue (double value);
    double value () const;

    void setRange (double min, double max);
    void setMaximumRange (double max);
    void setMinimumRange (double min);

signals:
    void valueChanged(double);
private:
    __private::DoubleValidator* val;
};

class IntNumberEdit : public NumberEdit
{
    Q_OBJECT
public:
    explicit IntNumberEdit(QWidget *parent = nullptr);
    void setValue (int value);
    int value () const;

    void setRange (int min, int max);
    void setMaximumRange (int max);
    void setMinimumRange (int min);

signals:
    void valueChanged(int);

private:
    __private::IntValidator* val;

};

#endif // NUMBEREDIT_H
