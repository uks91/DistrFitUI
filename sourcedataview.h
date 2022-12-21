#ifndef SOURCEDATAVIEW_H
#define SOURCEDATAVIEW_H

#include <QTableView>
#include <QtDebug>

class SourceDataView : public QTableView
{
    Q_OBJECT
public:
    explicit SourceDataView(QWidget *parent = 0);

signals:

public slots:

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event);
};

#endif // SOURCEDATAVIEW_H
