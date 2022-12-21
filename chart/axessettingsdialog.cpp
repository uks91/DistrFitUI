#include "axessettingsdialog.h"
#include "ui_axessettingsdialog.h"
#include <QtDebug>
#include "chart/axessettingsform.h"

static const QString defaultFontFamily    = "MS Shell Dlg 2";
//static constexpr int defaultFontPointSize = 8;
//static constexpr int defaultFontWeight    = 50;

SingleAxisSettings::SingleAxisSettings()
    : title {""}
    , tickCount{5}
    , labelsFont {QFont(defaultFontFamily, 13, QFont::Medium)}
    , pen  {QPen(QBrush(Qt::black), 3)}
    , titleFont {QFont(defaultFontFamily, 15, QFont::DemiBold)}
    , isAutoRange {true}
    , _minValue {0.0}
    , _maxValue {20.0}
{

}

double SingleAxisSettings::maxValue() const
{
    return _maxValue;
}

double SingleAxisSettings::minValue() const
{
    return _minValue;
}

void SingleAxisSettings::setMinMaxValue(double min, double max)
{
    _minValue = std::min(min, max);
    _maxValue = std::max(min, max);
}

void SingleAxisSettings::setMaxValue(double max)
{
    setMinMaxValue(_minValue, max);
}

void SingleAxisSettings::setMinValue(double min)
{
    setMinMaxValue(min, _maxValue);
}

AxesSettings::AxesSettings()
    : xAxis {}
    , yAxis {}
{
    xAxis.title = "Параметр";
    yAxis.title = "Плотность вероятности";
}

SingleAxisSettings &AxesSettings::x()
{
    return xAxis;
}

const SingleAxisSettings& AxesSettings::x() const
{
    return xAxis;
}

SingleAxisSettings &AxesSettings::y()
{
    return yAxis;
}

const SingleAxisSettings& AxesSettings::y() const
{
    return yAxis;
}

//!
//! \brief The AxesSettingsDialog::AxesSettingsDialogImpl class
//!

class AxesSettingsDialog::AxesSettingsDialogImpl
{
private:
    //AxesSettings settings;
    Ui::AxesSettingsDialog *ui;
    void refreshForm (AxesSettingsForm* form, const SingleAxisSettings &set);
    SingleAxisSettings readForm (AxesSettingsForm* form) const;

public:
    AxesSettingsDialogImpl (Ui::AxesSettingsDialog *ui);
    void setSettings (const AxesSettings &settings);
    AxesSettings getSettings () const;
};

void AxesSettingsDialog::AxesSettingsDialogImpl::refreshForm(AxesSettingsForm *form, const SingleAxisSettings &set)
{
    auto f = form->ui;
    f->txtTo   ->setValue(set.maxValue());
    f->txtFrom ->setValue(set.minValue());
    f->txtTicks->setValue(set.tickCount);
    f->txtTitle->setText(set.title);
    f->cbxAutoRange->setChecked(set.isAutoRange);

}

SingleAxisSettings AxesSettingsDialog::AxesSettingsDialogImpl::readForm(AxesSettingsForm *form) const
{
    SingleAxisSettings set;
    auto f = form->ui;

    set.setMinMaxValue(f->txtFrom->value(),
                       f->txtTo  ->value());
    set.tickCount = f->txtTicks  ->value();
    set.title     = f->txtTitle->text();
    set.isAutoRange = f->cbxAutoRange->isChecked();

    return set;
}


AxesSettingsDialog::AxesSettingsDialogImpl::AxesSettingsDialogImpl(Ui::AxesSettingsDialog *ui)
    : ui{ui}
{}

void AxesSettingsDialog::AxesSettingsDialogImpl::setSettings(const AxesSettings &settings)
{
    refreshForm(ui->tabHorizontal, settings.x());
    refreshForm(ui->tabVertical,   settings.y());
}

AxesSettings AxesSettingsDialog::AxesSettingsDialogImpl::getSettings() const
{
    AxesSettings settings;
    settings.x() = readForm(ui->tabHorizontal);
    settings.y() = readForm(ui->tabVertical);
    return settings;
}

//!
//! \brief AxesSettingsDialog::AxesSettingsDialog
//! \param parent
//!

AxesSettingsDialog::AxesSettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AxesSettingsDialog)
    , impl {std::make_unique<AxesSettingsDialogImpl>(ui)}
{
    ui->setupUi(this);
}

AxesSettingsDialog::~AxesSettingsDialog()
{
    delete ui;
}

void AxesSettingsDialog::setAxesSettings(const AxesSettings &settings)
{
    impl->setSettings(settings);
}

AxesSettings AxesSettingsDialog::getAxesSettings() const
{
    return impl->getSettings();
}

void AxesSettingsDialog::setVisibleTab(AxesSettingsDialog::Tab tab)
{
    ui->tabAxes->setCurrentIndex(tab);
}



