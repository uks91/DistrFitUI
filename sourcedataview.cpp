#include "sourcedataview.h"

SourceDataView::SourceDataView(QWidget *parent) : QTableView(parent)
{

}


void SourceDataView::keyPressEvent(QKeyEvent *event)
{
    qDebug () << "123";
    QTableView::keyPressEvent(event);
}
