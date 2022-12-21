#ifndef AXESSETTINGSDIALOG_H
#define AXESSETTINGSDIALOG_H

#include <QDialog>
#include <QPen>
#include <memory>

namespace Ui {
class AxesSettingsDialog;
}

struct SingleAxisSettings
{
    QString title;
    int tickCount;
    QFont labelsFont;
    QPen pen;
    QFont titleFont;
    bool isAutoRange;
    SingleAxisSettings();

    double maxValue() const;
    double minValue() const;
    void setMinMaxValue (double min, double max);
    void setMaxValue (double max);
    void setMinValue (double min);
private:
    double _minValue;
    double _maxValue;

};

class AxesSettings
{
private:
    SingleAxisSettings xAxis;
    SingleAxisSettings yAxis;
public:
    SingleAxisSettings& x();
    const SingleAxisSettings & x() const;
    SingleAxisSettings& y();
    const SingleAxisSettings& y() const;
    explicit AxesSettings();
};

class AxesSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    enum Tab {Horizontal, Vertical};
    explicit AxesSettingsDialog(QWidget *parent = nullptr);
    ~AxesSettingsDialog();

    void setAxesSettings (const AxesSettings &settings);
    AxesSettings getAxesSettings () const;

    void setVisibleTab (Tab tab);

private:
    Ui::AxesSettingsDialog *ui;

    class AxesSettingsDialogImpl;
    std::unique_ptr<AxesSettingsDialogImpl> impl;
    //AxesSettings settings;
};

#endif // AXESSETTINGSDIALOG_H
