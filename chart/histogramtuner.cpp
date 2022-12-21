// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "histogramtuner.h"
#include "ui_histogramtuner.h"
#include "histogramseries.h"
#include <algorithm>
#include "../managers/sourcedatamanager.h"

using std::for_each;

HistogramTuner::HistogramTuner(QWidget *parent)
    : QWidget(parent)
    , ui {new Ui::HistogramTuner}
    , bins {Bins()}
    //, histogram{std::make_shared<HistogramSeries>()}
    , histogram{new HistogramSeries()}
{
    ui->setupUi(this);

    histogram->setName("Histogram");

    QList<QRadioButton*> radButtons = this->findChildren<QRadioButton*>();

    for_each (radButtons.cbegin(), radButtons.cend(), [&] (QRadioButton* rad) {
        this->connect(rad, &QRadioButton::toggled, this, &HistogramTuner::slotRadButtonChanged);
    });

    QList<DoubleNumberEdit*> dblSpinBoxes = this->findChildren<DoubleNumberEdit*> ();
    for_each (dblSpinBoxes.cbegin(), dblSpinBoxes.cend(), [&] (DoubleNumberEdit* sbx) {
        if (sbx->objectName() != QString("sbxHistLineWidth"))
            this->connect(sbx, &DoubleNumberEdit::valueChanged, [&]{slotHistBinsChanged();});
    });

    QList<IntNumberEdit*> intSpinBoxes = this->findChildren<IntNumberEdit*> ();
    for_each (intSpinBoxes.cbegin(), intSpinBoxes.cend(), [&] (IntNumberEdit* sbx) {
        this->connect(sbx, &IntNumberEdit::valueChanged, [&]{slotHistBinsChanged();});
    });

    connect (ui->lblHistAreaColor, &ColorLabel::colorChanged, [&](const QColor &color){histogram->setColor(color);});
    connect (ui->lblHistLineColor, &ColorLabel::colorChanged, [&](const QColor &color){histogram->setBorderColor(color);});

    connect(ui->sbxHistLineWidth, &DoubleNumberEdit::valueChanged, [&] (double penWidth)
    {
        QPen pen = histogram->pen();
        pen.setWidthF(penWidth);
        histogram->setPen(pen);
    });

    connect(&SourceDataManager::instance(), &SourceDataManager::sgnlSourceDataChanged, this, &HistogramTuner::slotFitHistogramParameters);


    ui->lblHistAreaColor->setColor(QColor("#209fdf"));
    ui->lblHistLineColor->setColor(Qt::black);

    ui->radDx->toggle();
    ui->sbxHistLineWidth->setValue(1.0);
    ui->sbxHistLineWidth->setStep("0.1");
    ui->sbxHistLineWidth->setRange (0, 10);
    ui->sbxN->setMinimumRange(1);
    ui->sbxDx->setMinimumRange(0);
}

HistogramTuner::~HistogramTuner()
{
    //qDebug () << __PRETTY_FUNCTION__;
    delete ui;
    if (Q_UNLIKELY(histogram == nullptr)) // histogram is freed by chartViewer::chart::removeSeries()
        delete histogram;
}

Bins HistogramTuner::getBins() const
{
    return bins;
}

QRectF HistogramTuner::getHistogramRange() const
{
    return histogram->getRangeHint();
}

void HistogramTuner::slotValidateRange(bool value)
{
    //qDebug () << "slotValidateRange begins";
    QString text  = "";
    QString style = "color: ";
    QFont font = ui->lblHistState->font();
    if (value)
    {
        text = "Корректный диапазон";
        style += "black";
        font.setBold(false);
    }
    else
    {
        text = "Ошибка! Диапазон некорректный";
        style += "red";
        font.setBold(true);
    }
    // TODO:
    ui->lblHistState->setText(text);
    ui->lblHistState->setStyleSheet(style);
    ui->lblHistState->setFont(font);
    //qDebug () << "slotValidateRange ends";
}

void HistogramTuner::slotFitHistogramParameters()
{
    realVec data = SourceDataManager::instance().getSourceData();

    auto minmax = std::minmax_element(data.cbegin(), data.cend());
    double min = *(minmax.first);
    double max = *(minmax.second);

    lock(true);
    ui->radDx->setChecked(true);
    ui->sbxXMin->setValue(min);
    ui->sbxXMax->setValue(max);
    lock(false);
    this->histogram->setData(data);
    ui->sbxN   ->setValue(5);
}

void HistogramTuner::slotHistBinsChanged()
{
//    if (sender()==nullptr)
//        return;

//    qDebug () << sender()->objectName();

    lock(true);
    double xmin = ui->sbxXMin->value();
    double xmax = ui->sbxXMax->value();
    double dx   = ui->sbxDx  ->value();
    auto   n    = static_cast<short> (ui->sbxN   ->value());
    if (ui->radDx->isChecked())
    {
        bins = Bins::generateByVarDx(xmin, xmax, n);
        ui->sbxDx->setValue(bins.getDx());
    }
    else if (ui->radN->isChecked())
    {
        bins = Bins::generateByVarN(xmin, xmax, dx);
        ui->sbxN->setValue(bins.getN());
    }
    else if (ui->radXMax->isChecked())
    {
        bins = Bins::generateByVarXmax(xmin, dx, n);
        ui->sbxXMax->setValue(bins.getXmax());
    }
    else if (ui->radXMin->isChecked())
    {
        bins = Bins::generateByVarXmin(xmax, dx, n);
        ui->sbxXMin->setValue(bins.getXmin());
    }

    lock(false);
    histogram->setBins(getBins());

    slotValidateRange(histogram->isValidRange());
}

void HistogramTuner::slotRadButtonChanged(bool b)
{
    auto rad = qobject_cast<QRadioButton*> (sender());
//    if (sender() == nullptr)
    if (rad == nullptr)
        return;

    QString name = rad->objectName();
    name.replace("rad", "sbx");

    auto sbx = this->findChild<NumberEdit*> (name);
    if (sbx == nullptr) {
        return;}
    sbx->setEnabled(!b);
    return;

}

void HistogramTuner::lock(bool b)
{
    ui->sbxDx  ->blockSignals(b);
    ui->sbxN   ->blockSignals(b);
    ui->sbxXMax->blockSignals(b);
    ui->sbxXMin->blockSignals(b);
}
