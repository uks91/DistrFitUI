#ifndef PDFTUNER_H
#define PDFTUNER_H

#include <QWidget>
#include "global.h"

namespace Ui {
class PDFTuner;
}

class PDFSeries;
class DatasetManager;
class ResultsManager;


class PDFTuner : public QWidget
{
    Q_OBJECT

public:
    explicit PDFTuner(QWidget *parent = nullptr);
    ~PDFTuner();
    QRectF getPDFRange () const;

public slots:
    void slotSetDistrList     (CalcOption opt);
    void slotRebuildPDFSeries ();

private:
    Ui::PDFTuner *ui;
    DatasetManager &dsm;
    ResultsManager &res;

public:
    PDFSeries* pdf;
};

#endif // PDFTUNER_H
