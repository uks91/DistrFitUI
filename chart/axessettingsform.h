#ifndef AXESSETTINGSFORM_H
#define AXESSETTINGSFORM_H

#include <QWidget>
#include "ui_axessettingsform.h"

namespace Ui {
class AxesSettingsForm;
}

class AxesSettingsForm : public QWidget
{
    Q_OBJECT

public:
    explicit AxesSettingsForm(QWidget *parent = nullptr);
    ~AxesSettingsForm();

//private:
    Ui::AxesSettingsForm *ui;
};

#endif // AXESSETTINGSFORM_H
