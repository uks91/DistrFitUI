#ifndef DATASETDIALOG_H
#define DATASETDIALOG_H

#include <QDialog>
#include <QModelIndex>
#include "managers/datasetmanager.h"

class VariableManager;
class QItemSelectionModel;
class QStringListModel;

namespace Ui {
class DataSetDialog;
}

class DataSetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DataSetDialog(QWidget *parent = nullptr);
    ~DataSetDialog();

public slots:

    void slotAddNew();
    void slotCancel();
    void slotChangeDataSet(QModelIndex cur, QModelIndex);
    void slotDelete();
    void slotOk();
    void slotUpdate();

private:
    Ui::DataSetDialog *ui;

    QStringListModel    *model;
    QItemSelectionModel *selection;
    DatasetManager         &dsm;
    Dataset             curDataset;
//    QList<DataSet*>      mDatasets;
};

#endif // DATASETDIALOG_H
