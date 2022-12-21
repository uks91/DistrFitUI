// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "resultswidget.h"
#include "ui_resultswidget.h"
#include <QFileDialog>
#include <QMetaEnum>
#include <QLabel>
#include <QTextDocument>
#include <QClipboard>
#include <QTime>
#include <QStringListModel>
#include "managers/resultsmanager.h"
#include "managers/datasetmanager.h"
#include "managers/variablemanager.h"
#include "solver.h"
#include <QTextDocumentFragment>
#include <QTextTable>
#include "chartviewer.h"
#include "options.h"
//#include "managers/sourcedatamanager.h"

//!
//! \brief ResultsDelegate
//!

ResultsDelegate::ResultsDelegate (QObject *pobj) :
    QStyledItemDelegate (pobj)
{

}

void ResultsDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    if (index.column() < 2 || index.column() >= 6)
        return QStyledItemDelegate::paint(painter, option, index);


    bool isSelected = option.state.testFlag(QStyle::State_Selected);

    QTextDocument doc;
    QFont font;
    painter->save();
    font = option.font;
    font.setPointSize(8);

    if (isSelected)
    {
        painter->setPen(option.palette.color(QPalette::Highlight));
        painter->setBrush(option.palette.brush(QPalette::Highlight));
        painter->drawRect(option.rect);
        doc.setDefaultStyleSheet("div {color:white}");
    }
    else
    {
        painter->setPen(option.palette.color(QPalette::Base));
        painter->setBrush(qvariant_cast<QBrush> (index.data(Qt::BackgroundRole)));
        painter->drawRect(option.rect);
    }
    doc.setDefaultFont(font);

    QString htmlText;

    if (index.column() == 2)
    {
        // Колонка с параметрами распределений

//        QStringList list = index.data().toString().split(";", QString::SkipEmptyParts);
        auto list = index.data().toList();
        htmlText = "<div>";
        for (int i=0; i<list.size(); i++)
        {
            QString tmp = "<br />";
            if (i == list.size()-1)
                tmp = "";
            htmlText.append(QString("p<sub>%1</sub> = %2 %3").arg(i+1).arg(list.at(i).toString()).arg(tmp));
        }

        htmlText.append("</div>");
    }
    else
    {
        // колонка с результатами стат анализа

        QList<QVariant> d = index.data().toList();
        htmlText = "<div>";
        if (d.size() == 2)
        {
            htmlText = QString ("<div>S = %1<br />α = %2</div>")
                    .arg(d.at(0).toString())
                    .arg(d.at(1).toString());
        }
    }

    doc.setHtml(htmlText);

    painter->setPen(QPen(Qt::white, 0));
    painter->translate(option.rect.topLeft());
    doc.drawContents(painter);
    painter->restore();

}

QSize ResultsDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //qDebug () << "sizeHint...";
    if (index.column() == 2)
    {
        QFont font;
        font = option.font;
        font.setPointSize(8);
        QFontMetrics metr (font);
        int count = index.data().toString().split(";", QString::SkipEmptyParts).size();

        if (count < 2)
            count = 2;
        count++;

        return QSize (metr.width('w')*11, metr.height()*count); //11 - кол-во символов в строке
    }
    QSize size (120, 30);
    return size;
    return QStyledItemDelegate::sizeHint(option, index);
}

//QString html  (QString source/*, QSize size*/)
//{
//    QStringList list = source.split(";", QString::SkipEmptyParts);
//    QString htmlText = "<div>";
//    for (int i=0; i<list.size(); i++)
//    {
//        QString tmp = "<br />";
//        if (i == list.size()-1)
//            tmp = "";
//        htmlText.append(QString("p<sub>%1</sub> = %2 %3").arg(i+1).arg(list.at(i)).arg(tmp));
//    }
//    htmlText.append("</div>");
//    return htmlText;
//}

//!
//!
//!
//! CLASS ResultsModel
//!
//!
//!

ResultsModel::ResultsModel()
    : QAbstractTableModel ()
    , mgr {VariableManager::instance()}
    , dsm {DatasetManager::instance()}
    , res {ResultsManager::instance()}
//    , src {SourceDataManager::instance()}
{
    headers << "Распределение"
            << "Метод"
            << "Параметры"
            << "Колмогоров"
            << "Крамер-Мизес"
            << "Андерсон-Дарлинг"
            << "Вероятность"
            << "Сред. Вероятность"
            << "Сигма вероятности"
            << "Ниж. дов. граница";

    qRegisterMetaType<ResultsIndex>();


    connect (&res, &ResultsManager::sgnlAllResultsChanged,       this, &ResultsModel::slotSetModelData);
//    connect (&mgr, &VariableManager::sgnlSourceDataChanged,    this, &ResultsModel::slotSourceDataChanged);
//    connect (&dsm, &DatasetManager::sgnlCalcOptionChanged, this, &ResultsModel::slotUpdate);
    connect (&res, &ResultsManager::sgnlMapRestructed, this, &ResultsModel::slotUpdate);
    connect(&res, &ResultsManager::sgnlResultsChanged, this, &ResultsModel::slotResultReady);
    connect (&dsm, &DatasetManager::sgnlDataSetChanged, [this](){
        this->beginResetModel();
        this->beginResetModel();
    });
}

int ResultsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return headers.size();
}

QVariant ResultsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::ToolTipRole && role != Qt::BackgroundRole)
        return QVariant();

    int col = index.column();
    int row = index.row();

    auto resultIndex = res.getIndexByRow(row);
    auto resultValue = res.getResultByRow(row);

    if (!resultIndex.isValid())
        return QVariant();

    if (role == Qt::BackgroundRole)
    {
        if (col < 2)
            return QBrush();

        auto pair = qMakePair(resultIndex, col);
        if (!statuses.contains(pair))
            return QBrush();

        auto status = statuses[pair];

        switch (status)
        {
            case Waiting:
                return QBrush(Qt::yellow);
            case InProcess:
                return QBrush(Qt::green);
            default:
                return QBrush();
        }

        return QBrush(Qt::red);
    }

    const auto& distrs  = Options::getDistributions();
    const auto& methods = Options::getMethods();

    if (col == 0)
    {
        //! Распределение

        return distrs.at(resultIndex.getDistrId()).at(Options::NAME);
    }

    if (col == 1)
    {
        //! Метод оценки параметров

        return methods.at(resultIndex.getMethodId()).at(Options::NAME);
    }

    if (col == 2)
    {
        //! Параметры распределений

        QList<QVariant> out;
        for (const auto& elem : resultValue.pars)
            out.append(QVariant(numToString(elem,3)));
        return out;
    }

    if (col == 6)
    {
        //! Вероятность выхода за требования тз
        return numToString(resultValue.inrange, 8);

    }
    else if (col == 7)
    {
        //! Сигма вероятности выхода за тз
        return numToString(resultValue.inrangeMean, 8);
    }
    else if (col == 8)
    {
        //! Сигма вероятности выхода за тз
        return numToString(resultValue.inrangeSigma, 8);
    }
    else if (col == 9)
    {
        //! Нижняя доверительная граница
        return numToString(resultValue.inrangeBottom, 8);
    }
    else if (col >= 3 && col < columnCount())
    {
        int type = col-3;
        QList<QVariant> out = {0,0};
        if (resultValue.criteria.size() == 3 && resultValue.pvalues.size() == 3)
            out = {numToString(resultValue.criteria.at(type)),
                   numToString(resultValue.pvalues.at(type))};
        return out;
    }

    return QVariant();
}

//void ResultsModel::export2Math()
//{
//    QStringList list;
//    double tmp;
//    foreach (tmp, sourceData)
//    {
//        list << QString::number(tmp);
//    }
//    QString out = QString("kk={%1};\n").arg(list.join(", "));
//}

Qt::ItemFlags ResultsModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index);
}

QVariant ResultsModel::headerData(int section, Qt::Orientation orientation, int role) const
{

    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return headers.at(section);

    if (orientation == Qt::Vertical)
        return section+1;

    return QVariant();
}

bool ResultsModel::insertRows(int rowBefore, int count, const QModelIndex &parent)
{
    if (count <= 0)
        return false;

    beginInsertRows(parent, rowBefore, rowBefore+count-1);
    endInsertRows();
    return true;
}

bool ResultsModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (count <= 0)
        return false;

    beginRemoveRows(parent, row, row+count-1);
//    for (int i=0;i<count; i++)
//        values.removeAt(row);
    endRemoveRows();
    return true;
}

int ResultsModel::rowCount(const QModelIndex &/*parent*/) const
{
    return res.getMapSize();
//    return fields.size();
}

void ResultsModel::slotUpdate()
{
    // изменились Options; надо полностью обновить модель
    beginResetModel();
    removeRows(0, res.getMapSize());
    insertRows(0, res.getMapSize());
    endResetModel();
}

void ResultsModel::slotResultReady(ResultsManager::ResultType type, ResultsIndex idx)
{
    int row  = res.getRowByIndex(idx);

    int column = resultsTypeToColumnIndex(type);
    statuses[qMakePair(idx,column)] = Complete;

    emit dataChanged(index(row, column), index(row, column));
}

void ResultsModel::slotWaitingAll()
{
    statuses.clear();
    auto resIndexes = res.getIndexes();
    auto metaResType = QMetaEnum::fromType<ResultsManager::ResultType>();

    for (const auto& resIndex : resIndexes)
    {
        for (int i=0; i<metaResType.keyCount(); i++)
        {
            auto pair = qMakePair(resIndex, i+2);
            statuses[pair] = Waiting;
        }
    }
    emit dataChanged(index(0,2), index(rowCount(), columnCount()));
}

void ResultsModel::slotInProcess(ResultsIndex idx, ResultsManager::ResultType type)
{
    int column = resultsTypeToColumnIndex(type);
    statuses[qMakePair(idx,column)] = InProcess;
    auto modIdx = index (res.getRowByIndex(idx), column);
    emit dataChanged(modIdx,modIdx);
}

int ResultsModel::resultsTypeToColumnIndex(ResultsManager::ResultType type)
{
    switch (type)
    {
        case ResultsManager::DISTR_PARS:
            return 2;
        case ResultsManager::KOLMOGOROV:
            return 3;
        case ResultsManager::CRAMER_MISES:
            return 4;
        case ResultsManager::ANDERSON:
            return 5;
        case ResultsManager::PROB_INRANGE:
            return 6;
        case ResultsManager::PROB_INRANGE_MEAN:
            return 7;
        case ResultsManager::PROB_INRANGE_SIGMA:
            return 8;
        case ResultsManager::PROB_INRANGE_BOTTOM:
            return 9;
        default:
            return 0;
    }
}

void ResultsModel::slotSetModelData()
{
    // Устанаваливаем данные модели - результаты, параметры и т.д.
    // send - "ОТПРАВИТЕЛЬ" слота
    // Если 0 - ...
    // Если 1 - ...

//    if (send != 0)
//        return ;

    emit dataChanged(index(0,0), index(rowCount()-1,columnCount()-1));

}

//void ResultsModel::slotSourceDataChanged()
//{
//    VariableManager *dataManager = &VariableManager::instance();
//    sourceData = dataManager->getSourceData();
//}



//!
//!
//!
//! CLASS RESULTSWIDGET
//!
//!
//!

ResultsWidget::ResultsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ResultsWidget)
    , model {new ResultsModel ()}
    , del {new ResultsDelegate ()}
    , sol {std::make_shared<Solver>()}
    , chart {std::make_shared<ChartViewer> ()}
{
    ui->setupUi(this);

    ui->btnShowReport->setVisible(false);
    ui->btnExport->setVisible(false);

    ui->tblResults->setModel(model);
    del->setParent(ui->tblResults);
    ui->tblResults->setItemDelegate(del);
    ui->tblResults->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tblResults->verticalHeader()  ->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->sbxThreadsCount->setMaximum(QThread::idealThreadCount());
    ui->sbxThreadsCount->setValue(ui->sbxThreadsCount->maximum());
    ui->radHypShitikov->hide();

    connect (sol.get(),          &Solver::sgnlUpdateProgress, ui->progress, &QProgressBar::setValue);
    connect (sol.get(),          &Solver::sgnlWaitingAll,     model,        &ResultsModel::slotWaitingAll);
    connect (sol.get(),          &Solver::sgnlInProcess,      model,        &ResultsModel::slotInProcess);
    connect (sol.get(),          &Solver::started, [&] {time.start();});
    //connect (sol.get(),          &Solver::finished, [&] {qDebug () << time.elapsed();});

    connect (ui->btnCancel, &QAbstractButton::clicked,   sol.get(),         &Solver::slotCancel);

    connect (ui->cbxRound,       &QCheckBox::toggled,      [=] (bool status) {
        ui->lblNumDigits->setEnabled(status);
        ui->sbxNumDigits->setEnabled(status);
//        sol->setDigits(status ? ui->sbxNumDigits->value() : 0);
    });

    connect (ui->cbxItervalEstimation, &QCheckBox::toggled, [=] (bool status)
    {
        QList<QWidget*> list = {ui->txtInrangeNumIters, ui->txtToleranceProb, ui->lblInrangeNumIters, ui->lblToleranceProb};
        for (auto item : list)
            item->setEnabled(status);

    });

    ui->cbxItervalEstimation->setChecked(true);

    connect(ui->btnExportHTML, &QPushButton::clicked, this, &ResultsWidget::slotExportHTML);
    ui->txtNumIters->setValue(10000);
    ui->txtToleranceProb->setValue(0.9);
    ui->txtToleranceProb->setRange(0.0, 1.0);
    ui->txtInrangeNumIters->setValue(10000);
}

ResultsWidget::~ResultsWidget()
{
    delete ui;
    sol->quit();
}


void ResultsWidget::slotCalculate()
{
    if (sol->isRunning())
    {
        //qDebug () << "Solver is running!";
        return;
    }

    QList<QRadioButton*> radButtons = QList<QRadioButton*>  {
            ui->radHypSimple,
            ui->radHypLemeshko,
            ui->radHypShitikov};

    auto hyp = Fitting::Simple;
    for (auto iter = radButtons.cbegin(); iter != radButtons.cend(); ++iter)
    {
        if ((*iter)->isChecked())
        {
            hyp = static_cast<Fitting::Hypothesis>(iter-radButtons.cbegin());
            break;
        }
    }

    auto numItersChanged = [=] (int value)
    {
        int numIters = value;
        (numIters <=0) ? numIters=10000 : 0;
//        qDebug () << "num Iters: " << numIters;
//        sol->setDigits(numIters);
        return numIters;
    };

    sol->setHypothesis(hyp);
    sol->setDigits(ui->cbxRound->isChecked() ? ui->sbxNumDigits->value() : 0);
    sol->setNumIters(numItersChanged(ui->txtNumIters->value()));
    uint numItersForInrange = ui->cbxItervalEstimation->isChecked() ? numItersChanged(ui->txtInrangeNumIters->value()) : 0;
    sol->setNumItersForInrange(numItersForInrange);
    sol->setThreadCount(ui->sbxThreadsCount->value());
    sol->start();
}

void ResultsWidget::slotExport2Math()
{
    QClipboard *clip = QApplication::clipboard();
    QString str = sol->mathString();
    clip->setText(str);
}

void ResultsWidget::slotShowChart()
{
    //ChartViewer chart;
    if (chart == nullptr)
    {
        chart = std::make_shared<ChartViewer> ();
    }
    chart->show();
}

void ResultsWidget::slotUpdate()
{
    //qDebug () << sender()->objectName();
    //model->slotUpdate();
}

void ResultsWidget::slotExportHTML()
{
    //! CRUTCH
    // TODO: remake to special class

    auto fileName = QFileDialog::getSaveFileName(this, "Экспорт в HTML", "", "*.html");
    if (fileName.isEmpty())
        return;
    QTextDocument document;
    auto font = QFont ("Times New Roman", 12);
    document.setDefaultFont(font);

    QTextCursor cur (&document);
    font.setBold(true);
    auto normalFormat = cur.blockCharFormat();
    auto boldFormat = normalFormat;
    boldFormat.setFont(font);

    QTextTableFormat tableFormat;
    tableFormat.setBorder(1);
    tableFormat.setCellSpacing(0);
    tableFormat.setCellPadding(1);

    auto tbl = cur.insertTable(model->rowCount()+1, model->columnCount(), tableFormat);

    auto headers=model->getHeaders();

    std::for_each(headers.cbegin(), headers.cend(), [&](const QString &str)
    {
        cur.insertText(str);
        cur.movePosition(QTextCursor::NextCell, QTextCursor::MoveAnchor);
    });

    // TODO: don't use model! Use Managers instead

    for (int i=0; i<model->rowCount(); i++)
    {
        for (int j=0; j<model->columnCount(); j++)
        {
            QModelIndex idx = model->index(i, j);
            auto data = model->data(idx, Qt::DisplayRole);
            QTextDocumentFragment fragment;
//            qDebug () << i << j << model->data(idx, Qt::DisplayRole);
            if (j == 2) // Parametrs
            {
                auto list = data.toStringList();
                int k = 1;
                QString str;
                for (const auto &elem: list)
                {
                    str += QString("p<sub>%1</sub> = %2 <br/>").arg(k).arg(elem);
                    k++;
                }
                fragment = QTextDocumentFragment::fromHtml(str);
            }
            else if (j >= 3 && j < 6)
            {
                auto list = data.toList();
                if (list.size() != 2)
                {
                    continue;
                }
                auto str = QString("S = %1<br/>α = %2").arg(list.at(0).toString()).arg(list.at(1).toString());
                fragment = QTextDocumentFragment::fromHtml(str);
            }
            else if (j == 0)
            {
                auto currentText = data.toString();
                auto previousText = model->data(model->index(i-1, j), Qt::DisplayRole).toString();
                if (currentText != previousText)
                    fragment = QTextDocumentFragment::fromPlainText(currentText);
            }
            else if (j == 1)
            {
                auto currentText = data.toString();
                auto outText = currentText;
                if (currentText == "Минимального расстояния")
                    outText = "MD";
                else if (currentText == "Максимального правдоподобия")
                    outText = "ММП";
                else
                {
                    //qDebug () << currentText << (currentText == "Максимального правдоподобия");
                }
                fragment = QTextDocumentFragment::fromPlainText(outText);
            }
            else
            {
                fragment = QTextDocumentFragment::fromPlainText(data.toString());

            }
            cur.insertFragment(fragment);
            cur.movePosition(QTextCursor::NextCell, QTextCursor::MoveAnchor);
        }
    }

    // Merging cells

    const BoolVec boolvec = DatasetManager::instance().getCalcOption(Method);
    const int countMethods = std::accumulate(boolvec.cbegin(), boolvec.cend(), 0);

    for (int i=1; i<model->rowCount(); i+=countMethods)
    {
        tbl->mergeCells(i, 0, countMethods, 1);

    }

    auto htmlStr = document.toHtml();
    if (!fileName.endsWith(".html"))
        fileName += ".html";
    QFile file (fileName);
    file.open(QFile::WriteOnly);
    file.write(htmlStr.toUtf8());
}

//void ResultsWidget::slotHypothethisChanged()
//{
//    auto senderName = sender()->objectName();
//}
