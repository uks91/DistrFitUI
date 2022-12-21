// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "colorlabel.h"
#include <QMouseEvent>
#include <QColorDialog>

ColorLabel::ColorLabel(QWidget *parent, Qt::WindowFlags f)
    : QLabel(parent, f)
{
    setColor(QColor(Qt::black));
}

ColorLabel::ColorLabel(const QString &text, QWidget *parent, Qt::WindowFlags f)
    : QLabel(text, parent, f)
{
    setColor(QColor(Qt::black));
}

QColor ColorLabel::getColor() const
{
    return color;
}

void ColorLabel::setColor(const QColor &value)
{
    if (!value.isValid())
        return;
    color = value;
    this->setText(color.name());

    QColor textColor;

//    textColor.setRed  (255-color.red());
//    textColor.setGreen(255-color.green());
//    textColor.setBlue (255-color.blue());
    int gray = value.value();
    int sat  = value.saturation();
    int hue  = value.hue();
    gray = gray > 128 ? 0 : 255;
    sat  = sat  > 128 ? 0 : 255;
    hue  = hue  > 180 ? 0 : 359;
    hue = 359-value.hue();
    textColor.setHsv(hue,sat,gray);


    setStyleSheet(QString("background-color: %1;color: %2;").arg(color.name(), textColor.name()));

    emit colorChanged(color);
}

void ColorLabel::setText(const QString &txt)
{
    Q_UNUSED(txt);
    //setColor(color);
    QLabel::setText(color.name());
}

void ColorLabel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        setColor(QColorDialog::getColor(color, qobject_cast<QWidget*>(this->parent()), "Выберите цвет"));
    }
    QLabel::mousePressEvent(event);
}
