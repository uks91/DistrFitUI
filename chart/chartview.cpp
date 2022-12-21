#include "chartview.h"
#include <QtDebug>
#include "chart/axessettingsdialog.h"

//#define __TTT(TYPE, VALUE) if (qgraphicsitem_cast< TYPE *> (VALUE) != nullptr) return QString(#TYPE);

class ChartView::ChartViewImpl
{
public:
    ChartViewImpl(/*ChartView* parent*/)
        //: parent {parent}
    {
    }

    /*enum Types {
        TickNumber = 8
        , Axis = 10
        , AxisLine = 6
    };*/


    /*QString getTypeName (QGraphicsItem* item) const
    {
        __TTT (QGraphicsPathItem, item);
        __TTT (QGraphicsLineItem, item);
        __TTT (QGraphicsPolygonItem, item);
        __TTT (QGraphicsRectItem, item);
        __TTT (QGraphicsTextItem, item);
        //__TTT (QGraphic, item);
        return "unknown";
    }*/

    PlotObject getTypeOfItem (QGraphicsItem* item, const QPoint &position, const QRectF &plotArea)
    {
        //qDebug () << "Parent item:"  << item->parentItem() << item->parentItem()->type() << getTypeName(item->parentItem());
        //qDebug () << "Current item:" << item << item->type() << getTypeName(item);
        //return;


        QGraphicsTextItem* textItem = qgraphicsitem_cast<QGraphicsTextItem*>(item);
        if (textItem != nullptr)
        {
            if (textItem->parentItem()->type() == 10)
            {
                // axis
                if (position.x() < plotArea.left())
                    return YAxis;
                else if (position.y() > plotArea.bottom())
                    return XAxis;

            }
            else if (textItem->parentItem()->type() == 11)
            {
                //title
                auto obj = textItem->parentObject();
                QChart* chart = qgraphicsitem_cast<QChart*> (obj);
                auto axisX = chart->axes(Qt::Horizontal).at(0);
                auto axisY = chart->axes(Qt::Vertical  ).at(0);
                if (axisX->titleText() == textItem->toPlainText())
                    return XTitle;

                if (axisY->titleText() == textItem->toPlainText())
                    return  YTitle;

            }
        }
        else
        {

        }
        return None;

    }

/*private:
    ChartView* parent;*/
};

ChartView::ChartView(QWidget *parent)
    : QChartView (parent)
    , impl {std::make_shared<ChartViewImpl> ()}
{

}

ChartView::ChartView(QChart *chart, QWidget *parent)
    : QChartView (chart, parent)
    , impl {std::make_shared<ChartViewImpl> ()}
{

}

void ChartView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QGraphicsItem* item = itemAt(event->localPos().toPoint());
    //qDebug () << impl->getTypeOfItem(item, event->pos(), chart()->plotArea());
    auto result = impl->getTypeOfItem(item, event->pos(), chart()->plotArea());
    if (result != None)
        emit sgnlChartSettingsTrigered(result);
    QChartView::mouseDoubleClickEvent(event);
}

void ChartView::contextMenuEvent(QContextMenuEvent *event)
{
    QChartView::contextMenuEvent(event);
}
