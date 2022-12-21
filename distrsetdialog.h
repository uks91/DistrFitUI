#ifndef DISTRSETDIALOG_H
#define DISTRSETDIALOG_H

#include <QDialog>
#include "global.h"

class QStringListModel;
class QItemSelectionModel;
class VariableManager;
class DatasetManager;

namespace Ui {
class DistrSetDialog;
}

class DistrSetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DistrSetDialog(QWidget *parent = nullptr);
    ~DistrSetDialog();

public slots:
    void slotManageLists();
    void slotCalcOptionChanged     (CalcOption opt);
    void slotAccepted();

private:
    Ui::DistrSetDialog *ui;
    QVector<bool>  boolDistrs;
    QStringListModel *modAvailable;
    QStringListModel *modApplied;
    QItemSelectionModel *selAvailable;
    QItemSelectionModel *selApplied;
    VariableManager        &mgr;
    DatasetManager     &dsm;


    void updateLists ();
};

#endif // DISTRSETDIALOG_H
