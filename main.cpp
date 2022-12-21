// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "mainwindow.h"
#include <QApplication>
#include "extra_widgets/numberedit.h"

int main(int argc, char *argv[])
{
    //qSetMessagePattern ("[%{type}] (%{file}:%{line}) - %{message}");
    QApplication a(argc, argv);
    //qDebug () << QCoreApplication::applicationVersion();
    QCoreApplication::setApplicationVersion("1.3");
    MainWindow w;
    w.show();

    /*DoubleNumberEdit edt;
    edt.setRange(0,7);
    edt.setValue(-1);
    edt.show();*/

    return a.exec();
}
