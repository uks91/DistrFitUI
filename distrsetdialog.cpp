// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "distrsetdialog.h"
#include "ui_distrsetdialog.h"
#include "managers/datasetmanager.h"
#include "managers/variablemanager.h"
#include <QStringListModel>
#include <QItemSelectionModel>
#include "global.h"
#include "options.h"

DistrSetDialog::DistrSetDialog(QWidget *parent)
    : QDialog(parent)
    , ui           {new Ui::DistrSetDialog}
    , modAvailable {new QStringListModel ()}
    , modApplied   {new QStringListModel ()}
    , selAvailable {new QItemSelectionModel (modAvailable)}
    , selApplied   {new QItemSelectionModel (modApplied)}
    , mgr          {VariableManager::instance()}
    , dsm          {DatasetManager::instance()}
{
    ui->setupUi(this);
    ui->groupBox_2->setVisible(false);

    connect (ui->btnAdd,        &QPushButton::clicked,       this, &DistrSetDialog::slotManageLists);
    connect (ui->btnAddAll,     &QPushButton::clicked,       this, &DistrSetDialog::slotManageLists);
    connect (ui->btnBox,        &QDialogButtonBox::accepted, this, &DistrSetDialog::slotAccepted);
    connect (ui->btnBox,        &QDialogButtonBox::rejected, this, &DistrSetDialog::reject);
    connect (ui->btnTakeOut,    &QPushButton::clicked,       this, &DistrSetDialog::slotManageLists);
    connect (ui->btnTakeOutAll, &QPushButton::clicked,       this, &DistrSetDialog::slotManageLists);

    connect (&dsm, &DatasetManager::sgnlCalcOptionChanged, this, &DistrSetDialog::slotCalcOptionChanged);

    ui->lstAvailable->setModel         (modAvailable);
    ui->lstAvailable->setSelectionModel(selAvailable);
    ui->lstApplied  ->setModel         (modApplied);
    ui->lstApplied  ->setSelectionModel(selApplied);

    updateLists();

    selAvailable->select(modAvailable->index(0,0), QItemSelectionModel::Select);
    selApplied  ->select(modApplied  ->index(0,0), QItemSelectionModel::Select);
}

DistrSetDialog::~DistrSetDialog()
{
    delete ui;
}

void DistrSetDialog::slotManageLists()
{
    QObject* send = sender();
    if (send == nullptr)
        return ;
    QList<QStringList> distrs = Options::getDistributions();

    if (send->objectName()=="btnTakeOutAll")
        dsm.setCalcOption(CalcOption::Distribution, BoolVec (distrs.count(), false));

    else if (send->objectName()=="btnTakeOut" || send->objectName()=="btnAdd")
    {
        QModelIndexList selectedIndexes;
        bool            value;
        BoolVec         boolDistrs = dsm.getCalcOption(CalcOption::Distribution);

        if (send->objectName()=="btnTakeOut")
        {
            selectedIndexes = selApplied->selectedRows();
            value   = true;
        }
        else
        {
            selectedIndexes = selAvailable->selectedRows();
            value   = false;
        }

        QList<int> rows;
        for (const auto &elem : selectedIndexes)
            rows << elem.row();

        int numTrue = -1;
        for (int i=0; i<boolDistrs.count(); i++)
        {
            if (boolDistrs.at(i) == value)
            {
                numTrue++;
                if (rows.contains(numTrue))
                   boolDistrs[i] = !value;
            }
        }

        dsm.setCalcOption(CalcOption::Distribution, boolDistrs);
    }

    else if (send->objectName()=="btnAddAll")
        dsm.setCalcOption(CalcOption::Distribution, BoolVec (distrs.count(), true));

    else
        return;

    updateLists();
}

void DistrSetDialog::slotCalcOptionChanged(CalcOption opt)
{
    BoolVec tmp;

    if (opt == CalcOption::Method)
    {
        tmp = dsm.getCalcOption(opt);
        ui->cbxMD       ->setChecked(tmp.at(0));
        ui->cbxMLH      ->setChecked(tmp.at(1));
        ui->cbxSimpleMLH->setChecked(tmp.at(2));
    }
    else if (opt == CalcOption::Distribution)
        updateLists();
}

void DistrSetDialog::slotAccepted()
{
    BoolVec tmp = dsm.getCalcOption(CalcOption::Method);
//    qDebug () << ui->cbxMD->isCheckable() << ui->cbxMD->isChecked();
    tmp[0] = ui->cbxMD->isChecked();
    tmp[1] = ui->cbxMLH->isChecked();
    tmp[2] = ui->cbxSimpleMLH->isChecked();
    dsm.setCalcOption(CalcOption::Distribution, boolDistrs);

    dsm.setCalcOption(CalcOption::Method, tmp);
    dsm.saveCalcOptions();
    accept();
}

//!
//! PRIVATE
//!


void DistrSetDialog::updateLists()
{
    QStringList listAvailable, listApplied;

    QList<QStringList> distrs     = Options::getDistributions();
    /*BoolVec*/            boolDistrs = dsm.getCalcOption(CalcOption::Distribution);
//    qDebug () << boolDistrs;
    //boolDistrs.resize(distrs.size());

//    for (int i=0; i<distrs.size(); i++)
    int i=0;
    for (const auto &elem : distrs)
    {
        if (boolDistrs.at(i))
            listApplied << elem.at(Options::NAME);
        else
            listAvailable  << elem.at(Options::NAME);
        i++;
    }

    modAvailable->setStringList(listAvailable);
    modApplied->setStringList(listApplied);

    BoolVec methods = dsm.getCalcOption(CalcOption::Method);
    methods.resize(3);
    ui->cbxMD       ->setChecked(methods[0]);
    ui->cbxMLH      ->setChecked(methods[1]);
    ui->cbxSimpleMLH->setChecked(methods[2]);
}
