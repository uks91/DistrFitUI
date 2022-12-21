// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "sourcedata.h"
#include <QClipboard>
#include <QGuiApplication>
#include <QMessageBox>
#include <QKeyEvent>
#include <numeric>
#include "extra_widgets/numberedit.h"
#include "managers/sourcedatamanager.h"
#include "managers/variablemanager.h"
#include <cmath>

SourceDataDelegate::SourceDataDelegate(QObject *pobj)
    : QItemDelegate(pobj)
{

}

QWidget *SourceDataDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //qDebug () << "createEditor...";
    /*DoubleNumberEdit *edt = new DoubleNumberEdit(parent);
    edt->setFrame (false);
    edt->setGeometry(option.rect);
    edt->resize(option.rect.size());
    edt->setMinimumSize(option.rect.size());
    edt->setMaximumSize(option.rect.size());
    QString v = index.data().toString().replace(",", ".");
    edt->setValue(v.toDouble());
    edt->selectAll();

    qDebug () << index.data();

    qDebug () << option;
    return edt;*/

    QWidget* baseEditor = QItemDelegate::createEditor(parent, option, index);
    //qDebug () << baseEditor->dynamicPropertyNames();
    return baseEditor;
    QLineEdit * editor = qobject_cast<QLineEdit*> (baseEditor);
    if (editor == nullptr )
        return baseEditor;


    //Validator* val = new Validator (editor);
    //editor->setValidator(val);
    editor->setFrame(false);
    //editor->setToolTip("sdfsdf");

    return editor;
}

void SourceDataDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    //qDebug () << "setEditorData";
    QItemDelegate::setEditorData(editor, index);
}

//!
//!
//!
//! CLASS SourceDataModel
//!
//!
//!

SourceDataModel::SourceDataModel()
    : src {SourceDataManager::instance()}
    , var {VariableManager::instance()}
{
//    connect (&src, &SourceDataManager::sgnlSourceDataChanged, this, &SourceDataModel::slotSetSource);
    connect (&var, &VariableManager::sgnlPropertyIdChanged, this, &SourceDataModel::slotSetSource);
}

int SourceDataModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 1;
}

QVariant SourceDataModel::data(const QModelIndex &idx, int role) const
{
    if (!idx.isValid())
        return QVariant();


    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        auto r = idx.row();
        return r<src.size() ? numToString(src.at(r)) : "";
//        if (index.row() < valuesStr.size())
//            return valuesStr.at(index.row());
//        else
//            return "";
    }

    return QVariant();
}

Qt::ItemFlags SourceDataModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    return index.isValid() ? (flags | Qt::ItemIsEditable) : flags;
}

//QString SourceDataModel::getSource()
//{
//    return valuesStr.join(";");
//}

QVariant SourceDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return "Значения";

    if (orientation == Qt::Vertical)
        return QAbstractItemModel::headerData(section, orientation, role);

    return QVariant();
}

//void SourceDataModel::insert(int rowBefore, const QString &value)
//{
//    insertRows(rowBefore, 1);
//    valuesStr[rowBefore] = value;
//}

void SourceDataModel::remove(int row)
{
    removeRows(row, 1);
}

void SourceDataModel::reset()
{
    beginResetModel();
    //removeRows(0,rowCount()-1);
    endResetModel();
}

int SourceDataModel::rowCount(const QModelIndex &/*parent*/) const
{
    return src.size()+1;
}

bool SourceDataModel::setData(const QModelIndex &idx, const QVariant &value, int role)
{
    if (!idx.isValid())
        return false;

    if (role != Qt::EditRole)
        return false;
    auto tmp = value.toString().replace(",", ".");

    if (tmp.isEmpty())
        return false;

    auto row = idx.row();
    if (row == src.size())
        insertRows(src.size(), 1);
    else if (row > src.size())
        return false;
    // BUG : when adding the data

//    if (row >= values/*Str*/.size()-1 && !value.toString().isEmpty())
//        insertRows(rowCount(idx), 1);

//    values/*Str*/[row] = value.toString().toDouble();
    bool b;
    auto realValue = tmp.toDouble(&b);
    if (!b)
        return false;

    src.setAt(row, realValue);
    emit dataChanged(idx, idx);
    return true;
}

QuickStat SourceDataModel::getQuickStat() const
{
    const auto srcData = src.getSourceData();
    const auto size = srcData.size();

    QuickStat stat;
    if (size == 0)
        return stat;
    //double mean {0.0};
    //double stdev {0.0};
    auto minmax_iter = std::minmax_element(srcData.cbegin(), srcData.cend());
    stat.min = *(minmax_iter.first);
    stat.max = *(minmax_iter.second);

    if (size < 2)
        return stat;

    stat.mean  = std::accumulate(srcData.cbegin(), srcData.cend(), 0.0)/size;
    stat.stdev = std::accumulate(srcData.cbegin(), srcData.cend(), 0.0, [stat] (real res, real value)
    {
        return res + pow(stat.mean - value, 2.0);
    })/(size-1.0);

    stat.stdev = sqrt (stat.stdev);

    return stat;
}

//!
//! PRIVATE
//!

bool SourceDataModel::insertRows(int rowBefore, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, rowBefore, rowBefore+count-1);
    // TODO: change to alogorithm (внести изменения в менеджер!)
    src.blockSignals(true);
    for (int i=0;i<count; i++)
    {
//        valuesStr.insert(rowBefore, "");
//        values.insert(rowBefore, 0);
        src.insert(rowBefore);
    }
    src.blockSignals(false);
    src.sgnlSourceDataChanged();
    endInsertRows();
    return true;
}

bool SourceDataModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row+count-1);
    // TODO: change to alogorithm  (внести изменения в менеджер!)
    src.blockSignals(true);
    for (int i=0;i<count; i++)
    {
//        valuesStr.removeAt(row);
//        values.removeAt(row);
        src.remove(row);
    }
    src.blockSignals(false);
    src.sgnlSourceDataChanged();
    endRemoveRows();
    return true;
}

void SourceDataModel::slotSetSource()
{
    beginResetModel();
    endResetModel();
}

//!
//!
//!
//! CLASS SourceDataView
//!
//!
//!

SourceDataView::SourceDataView(QWidget *parent)
    : QTableView(parent)
    , mod {new SourceDataModel ()}
    , sel {new QItemSelectionModel (mod)}
    , del {new SourceDataDelegate (this)}
{
    this->setModel(mod);
    this->setSelectionModel(sel);
    this->setItemDelegate(del);
}

//SourceDataView::~SourceDataView()
//{
//    delete mod;
//    delete sel;
//    this->~QTableView();
//}

QuickStat SourceDataView::getQuickStat() const
{
    //mod->getMeanStdev(mean, stdev);
    return mod->getQuickStat();

}

void SourceDataView::setModel(SourceDataModel *model)
{
    mod = model;
    connect (mod, &QAbstractItemModel::dataChanged, this, &SourceDataView::slotDataChanged);
    QTableView::setModel(model);
}

/*void SourceDataView::setSource(const QString &source)
{
    //mod->setSource(source);
    emit sgnlModified();
}

void SourceDataView::setSource(const realVec &source)
{
    QStringList list;
    for (const auto &elem : source)
        list.append(QString::number(elem));
    setSource(list.join(";"));
}*/

void SourceDataView::slotDataChanged(const QModelIndex &index)
{
    if (!index.isValid())
        return ;

    QKeyEvent* key = new QKeyEvent (QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier);
    keyPressEvent(key);
    delete key;
    emit sgnlModified();
}

void SourceDataView::slotDeleteSelected()
{
    if (sel->selectedRows().size() < 0)
        return;


//    if (sel->selectedRows().size() > 0)
//    {
    //qDebug () << sel->selectedRows();
        int row = sel->selectedRows().at(0).row();
        if (row == mod->rowCount()-1)
            return;
        mod->remove(row);
        if (row == 0)
            row++;
        sel->select(mod->index(row-1,0),QItemSelectionModel::Select);
    //}
    emit sgnlModified();
}

void SourceDataView::keyPressEvent(QKeyEvent *event)
{
    int k = event->key();
    bool isCtrl = event->modifiers() == Qt::ControlModifier;

    auto indexes = sel->selectedIndexes();
    if (isCtrl && k == Qt::Key_V && indexes.size() >= 1)
    {
        // Выполнить какие-либо действия
        QClipboard *buffer = QGuiApplication::clipboard();
        QString text = buffer->text();
        QStringList list = text.split("\n");
        auto index = indexes.at(0);
        bool withReplacement = index.row()+1 == mod->rowCount();
        if (!withReplacement)
        {
            auto result = QMessageBox::question(this,
                                                "Параметры встаки значений",
                                                "Вы хотите заменить уже имеющиеся значения?",
                                                QMessageBox::Yes,
                                                QMessageBox::No,
                                                QMessageBox::Cancel);
            switch (result)
            {
                case QMessageBox::Yes:
                    withReplacement = true;
                    break;
                case QMessageBox::No:
                    withReplacement = false;
                    break;
                default:
                    return;
            }
        }
        const auto topLeftSelected = index;
        for (const auto& value: list)
        {
            if (value.isEmpty())
                continue;
            auto row = index.row();
            if (!withReplacement)
                mod->insertRow(row);
            mod->setData(index, value, Qt::EditRole);
            index = mod->index(row+1, 0);
        }
        sel->setCurrentIndex(topLeftSelected, QItemSelectionModel::Select);
        sel->select(QItemSelection{topLeftSelected, index},QItemSelectionModel::ClearAndSelect);
    }
    else if (isCtrl && k == Qt::Key_C && indexes.size() >= 1)
    {
        //qDebug () << "CTRL+C";
        auto* buffer = QGuiApplication::clipboard();
        QVector<QString> list {indexes.size(), ""};
        std::transform (indexes.cbegin(), indexes.cend(), list.begin(), [&](const QModelIndex& idx)
        {
            return idx.data(Qt::DisplayRole).toString();
        });
        buffer->setText(QStringList::fromVector(list).join("\n"));
    }
    else if (k == Qt::Key_Enter || k == Qt::Key_Return)
    {
        QKeyEvent* key = new QKeyEvent (QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier);
        keyPressEvent(key);
        delete key;
    }
    else if (k == Qt::Key_Delete)
    {
        slotDeleteSelected();
    }
    else
    {
        QTableView::keyPressEvent(event);
    }
}

// CRUTCH!
void SourceDataView::reset2()
{
    mod->reset();
}

QuickStat::QuickStat()
    : mean  {0.0}
    , stdev {0.0}
    , max   {0.0}
    , min   {0.0}
{

}
