#ifndef RESULTSWIDGET_H
#define RESULTSWIDGET_H

#include <QWidget>
#include <QAbstractTableModel>
#include <QStyledItemDelegate>
//#include <QStringListModel>

#include <QTime>
//#include "solver.h"
#include "managers/resultsmanager.h"
//#include "chartviewer.h"

class VariableManager;
class DatasetManager;
class ChartViewer;
//QString html  (QString source);

//!
//! \brief The ResultsDelegate class
//!

class ResultsDelegate : public QStyledItemDelegate
{
public:
    ResultsDelegate (QObject* pobj = nullptr);

    // QAbstractItemDelegate interface
public:
    void  paint   (QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    QMap<QModelIndex, QSizeF> sizes;


};


//!
//! \brief The ResultsModel class
//!

class ResultsModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    ResultsModel ();

    int           columnCount (const QModelIndex &parent=QModelIndex()) const;
    QVariant      data        (const QModelIndex &index, int role) const;
//    void          export2Math ();
    Qt::ItemFlags flags       (const QModelIndex &index) const;
    QVariant      headerData  (int section, Qt::Orientation orientation, int role) const;
    bool          insertRows  (int rowBefore, int count, const QModelIndex &parent=QModelIndex());
    void          remove      (int row);
    bool          removeRows  (int row, int count, const QModelIndex &parent=QModelIndex());
    int           rowCount    (const QModelIndex &parent=QModelIndex()) const;
    const QStringList&  getHeaders ()const {return headers;}

public slots:
    void slotSetModelData();
//    void slotSourceDataChanged ();
    void slotUpdate      ();
    void slotResultReady (ResultsManager::ResultType type, ResultsIndex idx);
    void slotWaitingAll ();
    void slotInProcess  (ResultsIndex idx, ResultsManager::ResultType type);


private:
    enum Status {Complete=0, Waiting, InProcess, };

    QStringList  headers;
    realVec  sourceData;
    VariableManager   &mgr;
    DatasetManager &dsm;
    ResultsManager &res;
//    SourceDataManager &src;

    QMap<QPair<ResultsIndex,int>, Status> statuses;
    int resultsTypeToColumnIndex (ResultsManager::ResultType type);
};

//!
//! \brief The ResultsWidget class
//!

namespace Ui {
class ResultsWidget;
}

class Solver;

class ResultsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ResultsWidget(QWidget *parent = 0);
    ~ResultsWidget();

signals:
//    void sgnlResultsReady ();


public slots:

    void slotCalculate ();
    void slotExport2Math();
    void slotShowChart();
    void slotUpdate ();
    void slotExportHTML();

private:
    Ui::ResultsWidget *ui;
    ResultsModel     *model;

//    QVector<bool> distrs;
    ResultsDelegate *del;
    int           method;
    std::shared_ptr<Solver>       sol;
    std::shared_ptr<ChartViewer>   chart;
    QTime time;


};

#endif // RESULTSWIDGET_H
