#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QChartView>
#include <memory>

QT_CHARTS_USE_NAMESPACE


class ChartView : public QChartView
{
    Q_OBJECT
public:
    explicit ChartView (QWidget *parent = Q_NULLPTR);
    explicit ChartView (QChart *chart, QWidget *parent = Q_NULLPTR);

    enum PlotObject {
        XAxis
        , YAxis
        , XTitle
        , YTitle
        , None
    };

private:
    class ChartViewImpl;
    std::shared_ptr<ChartViewImpl> impl;

    // QWidget interface
protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

signals:
    void sgnlChartSettingsTrigered(PlotObject);
};

#endif // CHARTVIEW_H
