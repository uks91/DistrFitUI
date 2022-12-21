// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "chartviewer.h"
#include "extra_widgets/colorlabel.h"
#include "ui_chartviewer.h"
#include <QtWidgets/QColorDialog>
#include <QtWidgets/QFileDialog>
#include "chart/histogramseries.h"
#include "chart/chartview.h"
#include "chartseries.h"


//!
//!
//! \brief ChartViewer::ChartViewer
//!
//!


ChartViewer::ChartViewer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChartViewer)
    , axesSettings {}
    , chartView {new ChartView(new QChart())}
{

    ui->setupUi(this);

    // подготавливаем диаграммы

    ui->layChart->addWidget(chartView);
    chartView->setRenderHint(QPainter::Antialiasing);

    auto chart = chartView->chart();
    QValueAxis* axisX = new QValueAxis();
    QValueAxis* axisY = new QValueAxis();

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    axisX->setTitleVisible();
    axisY->setTitleVisible();
    chart->legend()->setVisible(false);
    chart->setLocalizeNumbers(true);

    auto histogram = ui->wgtHistTuner->histogram;
    auto pdf = ui->wgtPDFTuner->pdf;
    chart->addSeries(histogram);
    chart->addSeries(pdf);


    pdf->attachAxis(axisX);
    pdf->attachAxis(axisY);
    histogram->attachAxis(axisX);
    histogram->attachAxis(axisY);

    applyAxesChanges();


    //! Создание связей
    connect(histogram,   &HistogramSeries::sgnlModified,        this, &ChartViewer::slotSetRange);
    connect(ui->btnSave, &QAbstractButton::clicked,             this, &ChartViewer::slotSave);
    connect(chartView,   &ChartView::sgnlChartSettingsTrigered, this, &ChartViewer::slotAxesSettingsDialog);

    // Задаем начальные значения для виджетов

}

ChartViewer::~ChartViewer()
{
    delete ui;
}

void ChartViewer::applyAxesChanges()
{
    QList<QAbstractAxis*> axes = chartView->chart()->axes();
    if (axes.size() != 2)
    {
//        qDebug () << "The count of axes is " << axes.size();
        return;
    }

    auto applyAxisChanges = [] (QValueAxis *axis, const SingleAxisSettings& settings)
    {
        axis->setRange(settings.minValue(), settings.maxValue());
        axis->setLinePen(settings.pen);
        axis->setLabelsFont(settings.labelsFont);
        axis->setTitleFont(settings.titleFont);
        axis->setTitleText(settings.title);
        axis->setTickCount(settings.tickCount);
    };

    auto getValueAxisByAligment = [axes] (Qt::Alignment align)
    {
        //QValueAxis* abstractAxis{nullptr};
        auto axIter = std::find_if(axes.cbegin(), axes.cend(), [align](QAbstractAxis* axis)
        {
            return axis->alignment() == align;
        });
        auto valueAxis = qobject_cast<QValueAxis*>(*axIter);
//        if (valueAxis == nullptr)
//            qDebug () << "ValueAxis with aligment " << align << "doesn\'t exists!";
        return valueAxis;
    };

    auto applyAutoAxis = [] (SingleAxisSettings &set, double min, double max)
    {
        if (!set.isAutoRange)
            return;

        set.setMinMaxValue(min, max);
    };


    QRectF rectf = ui->wgtHistTuner->getHistogramRange()
            .united(ui->wgtPDFTuner->pdf->getRangeHint());
    applyAutoAxis(axesSettings.x(), rectf.left(),   rectf.right());
    applyAutoAxis(axesSettings.y(), rectf.bottom(), rectf.top());

    QValueAxis* axisX = getValueAxisByAligment(Qt::AlignBottom);
    QValueAxis* axisY = getValueAxisByAligment(Qt::AlignLeft);

    applyAxisChanges(axisX, axesSettings.x());
    applyAxisChanges(axisY, axesSettings.y());

    QRectF rectf2 {QPointF(axesSettings.x().minValue(), axesSettings.y().maxValue()),
                   QPointF(axesSettings.x().maxValue(), axesSettings.y().minValue())};

    ui->wgtPDFTuner->pdf->setVisibleRange(rectf2);
}

void ChartViewer::slotSave()
{
    const int    widthPx  = 1200;
    const int    heightPx = 800;
    const double widthCm  = 15;
    const double heightCm = widthCm*heightPx/widthPx;
    QString str = QFileDialog::getSaveFileName(this, "Сохранить", "", "*.jpg");

    if (str.isEmpty())
        return;

    const QSize oldSize       = chartView->size();
    QSize newSize       = oldSize;
    newSize.setWidth(newSize.height()*widthPx/heightPx);

     // changes ...
    chartView->resize(newSize);

    QImage img (QSize(widthPx, heightPx), QImage::Format_ARGB32_Premultiplied);
    QRect rect = QRect(QPoint(0,0), img.size());
    QPainter painter (&img);
    painter.setBrush(QBrush(Qt::white));
    painter.setPen(Qt::white);
    painter.drawRect(rect);
    chartView->render(&painter);
    img.setDotsPerMeterX(widthPx/widthCm*100);
    img.setDotsPerMeterY(heightPx/heightCm*100);
    //qDebug () << img.dotsPerMeterX() << img.dotsPerMeterY() << widthPx/widthCm/100;

    // rollback changes

    chartView->resize(oldSize);

    img.save(str, "JPG");

}


void ChartViewer::slotSetRange()
{    
    applyAxesChanges();
    /*qDebug () << "!!!!!!!!!!!!!" << __PRETTY_FUNCTION__ << rectf;
    if (rectf.isNull())
        return;

    auto applyAutoAxis = [this] (SingleAxisSettings &set, double min, double max)
    {
        if (!set.isAutoRange)
            return;

        set.minValue = min;
        set.maxValue = max;
        this->applyAxesChanges();
    };

    applyAutoAxis(axesSettings.x(), rectf.left(),   rectf.right());
    applyAutoAxis(axesSettings.y(), rectf.bottom(), rectf.top());*/
}

void ChartViewer::slotAxesSettingsDialog(int obj)
{
    auto dialog = std::make_shared<AxesSettingsDialog> (this);
    dialog->setModal(true);
    dialog->setAxesSettings(axesSettings);

    AxesSettingsDialog::Tab tab {AxesSettingsDialog::Horizontal};

    if (obj == ChartView::XAxis || obj == ChartView::XTitle)
    {
        tab = AxesSettingsDialog::Horizontal;
    }
    else if (obj == ChartView::YAxis || obj == ChartView::YTitle)
    {
        tab = AxesSettingsDialog::Vertical;
    }
    dialog->setVisibleTab(tab);


    if (dialog->exec() == 1)
    {
        //saveCalcOptions();
        this->axesSettings = dialog->getAxesSettings();
        applyAxesChanges();
    }
}

