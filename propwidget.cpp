// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "propwidget.h"
#include "ui_propwidget.h"
#include <QMouseEvent>
#include <QtDebug>
#include <QStringListModel>
#include <QMessageBox>

#include "managers/variablemanager.h"


//!
//!
//! PropView
//!
//!

PropView::PropView(QWidget *parent) :
    QListView (parent),
    mgr {VariableManager::instance()}
{

    connect (&mgr, &VariableManager::sgnlModified, this, &PropView::setModified);
}

void PropView::setModified(bool value)
{
    isModified = value;
}

void PropView::mousePressEvent(QMouseEvent *event)
{
    QModelIndex curIdx = currentIndex();
    QModelIndex newIdx = indexAt(event->localPos().toPoint());
    bool isCancelled = false;
    if (newIdx.isValid() && newIdx != curIdx)
        isCancelled = askForSaving();
    if (isCancelled)
        return;
    QListView::mousePressEvent(event);
}

void PropView::keyPressEvent(QKeyEvent *event)
{
    QModelIndex curIdx = currentIndex();
    QModelIndex newIdx = curIdx;

    switch (event->key())
    {
        case Qt::Key_Down:
            newIdx = model()->index(curIdx.row()+1, 0);
            break;
        case Qt::Key_Up:
            newIdx = model()->index(curIdx.row()-1, 0);
            break;
        case Qt::Key_PageDown:
            newIdx = model()->index(model()->rowCount()-1,0);
            break;
        case Qt::Key_PageUp:
            newIdx = model()->index(0,0);
            break;
    }

    bool isCancelled = false;
    if (newIdx.isValid() && newIdx != curIdx)
        isCancelled = askForSaving();
    if (isCancelled)
        return;

    QListView::keyPressEvent(event);
}

bool PropView::askForSaving()
{
    //true - cancelled, false - not cancelled

    if (!isModified)
        return false;

    int question;
    question = QMessageBox::question(this,
                                     QString("Парнас v%1").arg(QCoreApplication::applicationVersion()),
                                     "Сохранить изменения анализируемой величины?",
                                     QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);

    switch (question)
    {
        case QMessageBox::Yes:
            emit sgnlSave();
            break;
        case QMessageBox::Cancel:
            return true;

    }
    isModified = false;
    return false;
}


//!
//!
//! WIDGET
//!
//!

PropWidget::PropWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PropWidget),
    mgr {VariableManager::instance()}
{
    ui->setupUi(this);

    model       = new QStringListModel ();
    selProperty = new QItemSelectionModel (model);

    ui->lstProperty->setModel(model);
    ui->lstProperty->setSelectionModel(selProperty);
    selProperty->setObjectName("selProperty");

    connect (ui->btnAddProperty,    &QAbstractButton::clicked,            this, &PropWidget::slotAddProperty);
    connect (ui->btnDeleteProperty, &QAbstractButton::clicked,            this, &PropWidget::slotDeleteProperty);
    connect (ui->lstProperty,       &PropView::sgnlSave,                  this, &PropWidget::slotSaveProperty);
    connect (selProperty,           &QItemSelectionModel::currentChanged, this, &PropWidget::slotPropertyChanged);
    connect (&mgr,                   &VariableManager::sgnlPropListChanged,    this, &PropWidget::changeDataSet);
}

PropWidget::~PropWidget()
{
    delete ui;
}


void PropWidget::changeDataSet(int index)
{

    model->setStringList(mgr.namesList());
    selProperty->setCurrentIndex(model->index(index,0), QItemSelectionModel::Select);
}


void PropWidget::slotAddProperty()
{
    if (!mgr.addProperty())
    {
        //qDebug () << "Not added!";
    }
}

void PropWidget::slotDeleteProperty()
{
    // Удаляем параметр
    if (!mgr.deleteProperty())
    {
        //qDebug () << "Not Deleted";
    }

}

void PropWidget::slotPropertyChanged(const QModelIndex &cur, const QModelIndex &/*prev*/)
{
    // Изменение виджета справа после выбора параметра

    if (cur.isValid())
        mgr.changeProperty(selProperty->currentIndex().row());
    return ;
}

void PropWidget::slotSaveProperty()
{
    // Сохраняем значение параметра, результаты стат анализа

    if (selProperty->currentIndex().isValid())
        mgr.saveProperty();
}

