#include "axessettingsform.h"
//#include <QIntValidator>

AxesSettingsForm::AxesSettingsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AxesSettingsForm)
{
    ui->setupUi(this);
    //ui->txtTicks->setValidator(new QIntValidator);
    connect(ui->cbxAutoRange, &QCheckBox::toggled, [&](bool state)
    {
        ui->txtTo  ->setEnabled(!state);
        ui->txtFrom->setEnabled(!state);
    });
}

AxesSettingsForm::~AxesSettingsForm()
{
    delete ui;
}
