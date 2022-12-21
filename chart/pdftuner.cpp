#include "pdftuner.h"
#include "ui_pdftuner.h"
#include "options.h"
#include "managers/datasetmanager.h"
#include "chartseries.h"
#include "managers/resultsmanager.h"

PDFTuner::PDFTuner(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PDFTuner)
    , dsm {DatasetManager::instance()}
    , res {ResultsManager::instance()}
    , pdf {new PDFSeries()}
{
    ui->setupUi(this);
    pdf->setName("PDF");

    connect (&dsm,            &DatasetManager::sgnlCalcOptionChanged,    this, &PDFTuner::slotSetDistrList);  //! change to ResultsManager::MapRestructed!!!
    connect (ui->boxDistrs,   QOverload<int>::of(&QComboBox::currentIndexChanged), [this]
    {
        //pdf->setDistrId  (ui->boxDistrs ->currentData().toInt());
        this->slotRebuildPDFSeries();
    });
    connect (ui->boxMethods,  QOverload<int>::of(&QComboBox::currentIndexChanged), [this]
    {
        //pdf->setMethodId  (ui->boxMethods ->currentData().toInt());
        this->slotRebuildPDFSeries();
    });
    connect (ui->lblPDFColor, &ColorLabel::colorChanged,  [this] (const QColor &color)
    {
        pdf->setColor(color);
    });
    connect(ui->sbxLineWidth, &DoubleNumberEdit::valueChanged, [this](double width)
    {
        QPen pen = pdf->pen();
        pen.setWidthF(width);
        pdf->setPen(pen);
    });

    connect(&res, &ResultsManager::sgnlAllResultsChanged, this, &PDFTuner::slotRebuildPDFSeries);

    ui->lblPDFColor->setColor(QColor("#a0c872"));
    ui->sbxLineWidth->setValue(2.0);
    ui->sbxLineWidth->setRange (0, 10);
    ui->sbxLineWidth->setStep("0.1");
}

PDFTuner::~PDFTuner()
{
    delete ui;
//    if (Q_UNLIKELY(pdf != nullptr))
//        delete pdf;
}

QRectF PDFTuner::getPDFRange() const
{
    return pdf->getRangeHint();
}

void PDFTuner::slotSetDistrList(CalcOption opt)
{
    QComboBox          *box = nullptr;
    QList<QStringList> available;
    BoolVec            calc;

    if (opt == CalcOption::Distribution)
    {
        box       = ui->boxDistrs;
        available = Options::getDistributions();
        calc      = dsm.getCalcOption(CalcOption::Distribution);
    }
    else if (opt == CalcOption::Method)
    {
        box       = ui->boxMethods;
        available = Options::getMethods();
        calc      = dsm.getCalcOption(CalcOption::Method);
    }
    else
        return;

    box->clear();
    for (int i=0; i<calc.size(); i++)
        if (calc.at(i))
            box->addItem(available.at(i).at(1), i);
}

void PDFTuner::slotRebuildPDFSeries()
{
    int distrId  = ui->boxDistrs ->currentData().toInt();
    int methodId = ui->boxMethods ->currentData().toInt();

    auto pars = res.getResultByIndex(ResultsIndex(distrId, methodId)).pars;
    if (pars.isEmpty())
        return;

    pdf->setDistr(distrId, pars);
}
