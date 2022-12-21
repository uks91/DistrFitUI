#ifndef SOURCEDATA
#define SOURCEDATA

#include <QAbstractTableModel>
#include <QItemDelegate>
#include <QTableView>
#include "global.h"


class SourceDataManager;
class SourceDataManager;
class VariableManager  ;
//!
//! Делегат
//!

class SourceDataDelegate: public QItemDelegate
{
public:
    SourceDataDelegate (QObject* pobj = nullptr);

    // QAbstractItemDelegate interface
public:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
};

//!
//! Модель
//!

struct QuickStat
{
    QuickStat ();
    double mean;
    double stdev;
    double max;
    double min;
};

class SourceDataModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    SourceDataModel();

    int           columnCount (const QModelIndex &) const;
    QVariant      data        (const QModelIndex &idx, int role) const;
    Qt::ItemFlags flags       (const QModelIndex &index) const;
    QString       getSource   ();
    QVariant      headerData  (int section, Qt::Orientation orientation, int role) const;
//    void          insert      (int rowBefore, const QString &value);
    void          remove      (int row);
    void          reset ();
    int           rowCount    (const QModelIndex &parent=QModelIndex()) const;
    bool          setData     (const QModelIndex &idx, const QVariant &value, int role);
    QuickStat     getQuickStat () const;

//    void          setSource   (const QString &source);

private:
//    QStringList valuesStr;
    SourceDataManager &src;
    VariableManager &var;
//    realVec values;

    bool          insertRows  (int rowBefore, int count, const QModelIndex &parent=QModelIndex());
    bool          removeRows  (int row, int count, const QModelIndex &parent=QModelIndex());
    //void reset ();
private slots:
     void          slotSetSource   ();

};

//!
//! Виджет
//!

class SourceDataView : public QTableView
{
    Q_OBJECT
public:

    explicit SourceDataView(QWidget *parent = nullptr);
//    ~SourceDataView();

//    QString  getSource    ();
    QuickStat     getQuickStat () const;
    void     setModel     (SourceDataModel *model);
    //void     setSource    (const QString &source);
    //void     setSource    (const realVec &source);

signals:
    void sgnlModified();

public slots:
    void slotDataChanged   (const QModelIndex &index);
    void slotDeleteSelected();

protected:
    void keyPressEvent(QKeyEvent *event);

    // QAbstractItemView interface
public:


private:
    SourceDataModel *mod;
    QItemSelectionModel *sel;
    SourceDataDelegate *del;

    // QAbstractItemView interface
public:
    void reset2(); //CRUTCH!
};

#endif // SOURCEDATA
