// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <QTime>
#include <QMessageBox>
#include <QCoreApplication>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "managers/datasetmanager.h"
#include "managers/sourcedatamanager.h"
#include "managers/variablemanager.h"
#include "sourcedata.h"
#include "datasetdialog.h"
#include "distrsetdialog.h"

using std::shared_ptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mgr {VariableManager::instance()}
    , dsm {DatasetManager::instance()}
    , src {SourceDataManager::instance()}
{
    ui->setupUi(this);
    ui->btnExceptValue->setVisible(false);


    connect (ui->txtPropertyName, &QLineEdit::textEdited,              &mgr,  &VariableManager::setVariableName);
    connect (ui->btnSavePropertyChanges,
                                  &QPushButton::clicked,               this, &MainWindow::slotSaveProperty);

    connect (ui->btnDeleteValue,  &QPushButton::clicked,               ui->tblSourceData,
                                                                             &SourceDataView::slotDeleteSelected);

    connect (ui->tblSourceData,   &SourceDataView::sgnlModified,       this, &MainWindow::slotDataChanged);
    connect (&mgr,                &VariableManager::sgnlPropertyIdChanged, this, &MainWindow::slotChangeProperty);
    connect (&mgr,                &VariableManager::sgnlModified,          this, &MainWindow::slotModified);
    connect (&dsm,                &DatasetManager::sgnlDataSetChanged, this, &MainWindow::slotChangeDataSet);
    connect (ui->btnApply,        &QPushButton::clicked,   this, &MainWindow::slotApplyBoundaries);

    //connect(ui->menuAbout, SIGNAL(triggered), [](){});
    //ui->menuAbout

    actChooseDataSet  = ui->menuBar->addAction("Выбрать набор данных");
    actChooseDistrSet = ui->menuBar->addAction("Выбрать распределения");
    actAbout          = ui->menuBar->addAction("О программе");
    actChooseDistrSet->setEnabled(false);

    connect (actChooseDataSet,  &QAction::triggered, this, &MainWindow::slotChooseDataSetTriggered);
    connect (actChooseDistrSet, &QAction::triggered, this, &MainWindow::slotChooseDistrSetTriggered);
    connect (actAbout,          &QAction::triggered, this, &MainWindow::slotAbout);

    slotModified(false);

    QString title = "DistrFit v%1";
    this->setWindowTitle(title.arg(QCoreApplication::applicationVersion()));
    ui->btnAddValue->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//!
//! SLOTS
//!

void MainWindow::slotChooseDataSetTriggered()
{
    // Выбираем набор данных
    auto dataset = std::make_shared<DataSetDialog> (this);
    dataset->setModal(true);
    dataset->exec();
    //delete dataset;
}

void MainWindow::slotChooseDistrSetTriggered()
{
    // Выбираем набор распределений для проведения расчёта

    auto distrset = std::make_shared<DistrSetDialog> (this);
    distrset->setModal(true);

    if (distrset->exec() == 1)
    {
        //saveCalcOptions();

    }
}

void MainWindow::slotAbout()
{
    QMessageBox::about(this,
                       "О программе",
                       QString("Программа DistrFit v %1\n© Иванушкин Ю.С. (yuriy.ivanushkin@yandex.ru)\n2017-2020 г.").arg(QCoreApplication::applicationVersion()));
}


void MainWindow::slotDataChanged()
{
//    ui->tblSourceData
//    QString objName = sender ()->objectName();

//    if (objName == "tblSourceData")
//    {
    auto stat = ui->tblSourceData->getQuickStat();
    ui->lblMean ->setText(numToString(stat.mean,  5));
    ui->lblStdev->setText(numToString(stat.stdev, 5));
    ui->lblMin  ->setText(numToString(stat.min,   5));
    ui->lblMax  ->setText(numToString(stat.max,   5));
    slotModified(true);

        // WARNING: mgr.setSourceData was here;
        //mgr.setSourceData(ui->tblSourceData->getSource());
//    }
//    else if (objName == "results")
//    {

//    }

}

void MainWindow::slotSaveProperty()
{
    // Сохраняем значение параметра, результаты стат анализа

    mgr.saveProperty();

}

void MainWindow::slotChangeDataSet()
{
    enableWidgets();
    QString title = "DistrFit v%1: %2";
    this->setWindowTitle(title.arg(QCoreApplication::applicationVersion()).arg(dsm.name()));
}

void MainWindow::slotChangeProperty(int propertyId)
{
    // Обновляем виджеты справа
    Q_UNUSED(propertyId)
    ui->txtPropertyName->setText(mgr.getVariableName());

    auto lower = src.getLowerBoundary();
    auto upper = src.getUpperBoundary();

    auto lambda = [] (QCheckBox* box, DoubleNumberEdit* line, qreal value)
    {
        bool b = value == Const::min_limit;
//        line->setText(b ? "" : QString::number(value));
        line->setValue(b ? 0 : value);
        box->setChecked(!b);
    };
    lambda (ui->cbxLower, ui->txtLower, lower);
    lambda (ui->cbxUpper, ui->txtUpper, upper);

    ui->tabWidget->setEnabled(true);

    slotDataChanged();
    // WARNING: ui->tblSourceData->setSource(mgr.getSourceData());
}

void MainWindow::slotModified(bool b)
{
    isModified = b;
    ui->btnSavePropertyChanges    ->setEnabled(b);
    ui->btnRollbackPropertyChanges->setEnabled(b);
}

void MainWindow::slotApplyBoundaries()
{
    bool bothBoundaries {true};

    auto lambda = [&bothBoundaries] (QCheckBox* box, DoubleNumberEdit* line) -> real
    {
        bool b = box->isChecked();
        bothBoundaries = bothBoundaries && b;
        if (b)
        {
//            QString str = line->text().replace(",", ".");
//            return str.toDouble();
            return line->value();
        }
        else
            return Const::min_limit;
    };

    real lower = lambda (ui->cbxLower, ui->txtLower);
    real upper = lambda (ui->cbxUpper, ui->txtUpper);


    if (bothBoundaries && upper <= lower)
    {
        QMessageBox::information(this, "Границы диапазона", "Верхняя граница диапазона должна быть больше нижней!");
        return;
    }

    src.slotSetLowerBoundary(lower);
    src.slotSetUpperBoundary(upper);
}


//!
//! PRIVATE
//!

void MainWindow::closeEvent(QCloseEvent *event)
{

    QApplication::quit();
//    QWidget::closeEvent(event);
}

void MainWindow::enableWidgets()
{
    QList<QWidget*> listOfWidgets = {
        ui->wgtProp

    };

    if (!mgr.isCurIndexValid())
    {
        // Hides table and text fields after choosing the empty Dataset
        ui->cbxLower->setChecked(false);
        ui->cbxUpper->setChecked(false);
//        ui->txtLower->setText("");
//        ui->txtUpper->setText("");
        ui->txtLower->setValue(0);
        ui->txtUpper->setValue(0);
        ui->tabWidget->setEnabled(false);
        //ui->tblSourceData->model().
        ui->tblSourceData->reset2();
    }
    else
    {
        listOfWidgets.append(ui->tabWidget);
    }

    std::for_each(listOfWidgets.begin(), listOfWidgets.end(), [](QWidget* wgt)
    {
        wgt->setEnabled(true);
    });
    actChooseDistrSet->setEnabled(true);
}
