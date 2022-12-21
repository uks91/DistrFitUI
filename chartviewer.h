#ifndef CHARTVIEWER_H
#define CHARTVIEWER_H

//#include <QWidget>
#include <QtWidgets/QLabel>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include "global.h"

#include "chart/axessettingsdialog.h"

class ChartView;
class AxesSettings;

QT_CHARTS_USE_NAMESPACE

//!
//! ChartViewer::ChartViewer
//!

namespace Ui {
class ChartViewer;
}



class ChartViewer : public QWidget
{
    Q_OBJECT

public:
    explicit ChartViewer(QWidget *parent = nullptr);
    ~ChartViewer();
    // TODO: move to Pimpl
    void applyAxesChanges ();

public slots:
    void slotSave             ();
    void slotSetRange         ();

    void slotAxesSettingsDialog (int obj);

private:
    Ui::ChartViewer *ui;
    AxesSettings axesSettings;
    ChartView *chartView;

};

#endif // CHARTVIEWER_H
