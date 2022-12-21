// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <QMessageBox>
#include <QtDebug>
#include <QModelIndex>
#include <QStringListModel>
#include <QItemSelectionModel>
#include <QDateTime>
#include "datasetdialog.h"
#include "ui_datasetdialog.h"
#include "managers/variablemanager.h"
#include "managers/datasetmanager.h"

DataSetDialog::DataSetDialog(QWidget *parent)
    : QDialog(parent)
    , ui        {new Ui::DataSetDialog}
    , model     {new QStringListModel ()}
    , selection {new QItemSelectionModel(model)}
    , dsm       {DatasetManager::instance()}
{
    ui->setupUi(this);

    dsm.loadDatasets();
    model->setStringList(dsm.namesList());

    ui->lstDataSet->setModel(model);
    ui->lstDataSet->setSelectionModel(selection);
    connect(ui->lstDataSet, &QListView::doubleClicked, [this](const QModelIndex & index)
    {
        this->slotChangeDataSet(index, index);
        this->slotOk();
    });
    connect(selection,&QItemSelectionModel::currentChanged, this, &DataSetDialog::slotChangeDataSet);

    auto index = dsm.index();
    index = index < 0 ? 0 : index;
    selection->setCurrentIndex(model->index(index, 0), QItemSelectionModel::Select);
}

DataSetDialog::~DataSetDialog()
{
    dsm.unloadDatasets(false);
    delete ui;
}

//!
//! PUBLIC SLOTS
//!

void DataSetDialog::slotAddNew()
{
    if (!dsm.add())
    {
//        qDebug () << "Dataset isn\'t added";
        return;
    }
    model->setStringList(dsm.namesList());
    selection->setCurrentIndex(model->index(model->rowCount()-1, 0), QItemSelectionModel::Select);
}

void DataSetDialog::slotCancel()
{
    dsm.unloadDatasets(false);
    reject();
}

void DataSetDialog::slotChangeDataSet(QModelIndex cur, QModelIndex)
{
    if (!cur.isValid())
        return;

    dsm.setDatasetItem(cur.row());

    ui->txtComment->setPlainText(dsm.comment());
    ui->txtName   ->setText     (dsm.name());
    QDateTime datetime = QDateTime::fromString(dsm.date(), "yyyy-MM-dd HH:mm:ss");
    ui->lblDate   ->setText     (datetime.toString("dd.MM.yyyy, HH:mm:ss"));
}

void DataSetDialog::slotDelete()
{
    if (!dsm.remove())
    {
//        qDebug () << "Not removed!";
        return;
    }
    QModelIndex cur = selection->currentIndex();
    model->setStringList(dsm.namesList());
    if (cur.row() >= model->rowCount())
        cur = model->index(model->rowCount()-1, 0);
    selection->setCurrentIndex(cur, QItemSelectionModel::Select);
}

void DataSetDialog::slotOk()
{
    dsm.unloadDatasets(true);
    accept();
}

void DataSetDialog::slotUpdate()
{
    if (!dsm.update(ui->txtName->text(), ui->txtComment->toPlainText()))
    {
        QMessageBox::warning(this, "Сообщение", dsm.lastError(), QMessageBox::Ok);
        //qDebug () << "Not updated!" << dsm.lastError();
        return;
    }
    QModelIndex cur = selection->currentIndex();
    model->setStringList(dsm.namesList());
    selection->setCurrentIndex(cur, QItemSelectionModel::Select);
}
