#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class VariableManager;
class DatasetManager;
class SourceDataManager;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void sgnlChanged();
    void sgnlDataSetChanged (int);

private slots:

    void slotChooseDataSetTriggered();
    void slotChooseDistrSetTriggered();
    void slotAbout ();

    void slotDataChanged      ();
    void slotSaveProperty     ();
    void slotChangeDataSet  ();
    void slotChangeProperty (int propertyId);
    void slotModified (bool b);
    void slotApplyBoundaries ();
    //void slotSourceDataCleared();


private:
    Ui::MainWindow *ui;
    QAction *actChooseDistrSet;
    QAction *actChooseDataSet;
    QAction *actAbout;

    VariableManager &mgr;
    DatasetManager  &dsm;
    SourceDataManager &src;

    bool isModified;

    void closeEvent(QCloseEvent *event);
    void enableWidgets();


};

#endif // MAINWINDOW_H
