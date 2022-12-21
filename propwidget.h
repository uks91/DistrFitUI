#ifndef PROPWIDGET_H
#define PROPWIDGET_H

#include <QWidget>
#include <QListView>

class VariableManager;
class QStringListModel;

//!
//!

class PropView : public QListView
{
    Q_OBJECT
public:
    explicit PropView (QWidget *parent = Q_NULLPTR);

signals:
    void sgnlSave();

public slots:
    void setModified(bool value=true);

protected:
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    bool isModified;
    VariableManager &mgr;
    bool askForSaving ();
};

//!
//!

namespace Ui {
class PropWidget;
}

class PropWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PropWidget(QWidget *parent = nullptr);
    ~PropWidget();

public slots:
    void changeDataSet(int index);
    void slotAddProperty();
    void slotDeleteProperty();
    void slotPropertyChanged(const QModelIndex &cur, const QModelIndex &);

    void slotSaveProperty();


signals:


private:
    Ui::PropWidget *ui;

    QStringListModel *model;
    QItemSelectionModel *selProperty;
    VariableManager  &mgr;
    //DatasetManager *dsm;
};

#endif // PROPWIDGET_H
