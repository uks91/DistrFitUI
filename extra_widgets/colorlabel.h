#ifndef COLORLABEL_H
#define COLORLABEL_H

#include <QLabel>

class ColorLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ColorLabel (QWidget* parent=Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ColorLabel (const QString &text, QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());

    QColor getColor () const;
    void   setColor (const QColor &value);
    void   setText  (const QString &txt);

//    Q_PROPERTY (QColor color READ getColor WRITE setColor NOTIFY colorChanged)
signals:
    void colorChanged(QColor);
public slots:

private:
    QColor color;
    void mousePressEvent(QMouseEvent *event);

};

#endif // COLORLABEL_H
