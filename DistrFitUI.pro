#-------------------------------------------------
#
# Project created by QtCreator 2017-09-13T08:34:38
#
#-------------------------------------------------

QT       += core gui sql charts concurrent
CONFIG += c++14

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DistrFitUi
TEMPLATE = app


SOURCES += main.cpp\
    chart/axessettingsdialog.cpp \
    chart/axessettingsform.cpp \
    chart/chartview.cpp \
    chart/pdftuner.cpp \
        mainwindow.cpp \
    datasetdialog.cpp \
    distrsetdialog.cpp \
    resultswidget.cpp \
    options.cpp \
    solver.cpp \
#    sourcedataview.cpp
    sourcedata.cpp \
    propwidget.cpp \
    managers/dbmanager.cpp \
    chartviewer.cpp \
    chartseries.cpp \
    managers/datasetmanager.cpp \
    managers/resultsmanager.cpp \
    managers/variablemanager.cpp \
    task.cpp \
    managers/sourcedatamanager.cpp \
    extra_widgets/colorlabel.cpp \
    chart/histogramtuner.cpp \
    chart/histogramseries.cpp \
    chart/bins.cpp \
    chart/histogram.cpp \
    extra_widgets/numberedit.cpp \
    global.cpp \
    managers/errorhandler.cpp

HEADERS  += mainwindow.h \
    chart/axessettingsdialog.h \
    chart/axessettingsform.h \
    chart/chartview.h \
    chart/pdftuner.h \
    datasetdialog.h \
    distrsetdialog.h \
    fitting.h \
    resultswidget.h \ # \
    options.h \
    solver.h \
#    sourcedataview.h
    #distrs.h
    sourcedata.h \
    propwidget.h \
    managers/dbmanager.h \
    chartviewer.h \
    chartseries.h \
    managers/datasetmanager.h \
    global.h \
    managers/resultsmanager.h \
    managers/variablemanager.h \
    task.h \
    managers/sourcedatamanager.h \
    extra_widgets/colorlabel.h \
    chart/histogramtuner.h \
    chart/histogramseries.h \
    chart/bins.h \
    chart/histogram.h \
    extra_widgets/numberedit.h \
    managers/errorhandler.h

FORMS    += mainwindow.ui \
    chart/axessettingsdialog.ui \
    chart/axessettingsform.ui \
    chart/pdftuner.ui \
    datasetdialog.ui \
    distrsetdialog.ui \
    extra_widgets/test.ui \
    resultswidget.ui \
    propwidget.ui \
    chartviewer.ui \
    chart/histogramtuner.ui

####DistrLib


#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../builds/build-DistrLib/release/ -lDistrLib
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../builds/build-DistrLib/debug/ -lDistrLib
#else:unix:!macx: LIBS += -L$$PWD/../builds/build-DistrLib/ -lDistrLib

#INCLUDEPATH += $$PWD/../DistrLib/headers
#DEPENDPATH += $$PWD/../DistrLib/headers

#win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../builds/build-DistrLib/release/libDistrLib.a
#else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../builds/build-DistrLib/debug/libDistrLib.a
#else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../builds/build-DistrLib/release/DistrLib.lib
#else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../builds/build-DistrLib/debug/DistrLib.lib
#else:unix:!macx: PRE_TARGETDEPS += $$PWD/../builds/build-DistrLib/libDistrLib.a

DISTFILES += \
    model.qmodel \
    qwe.qmodel

STATECHARTS += \
    qwe.scxml

VERSION = 1.3
#VERSION_PE_HEADER = 1.3

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-DistrLib/release/ -lDistrLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-DistrLib/debug/ -lDistrLib
else:unix: LIBS += -L$$PWD/../build-DistrLib/ -lDistrLib

INCLUDEPATH += $$PWD/../DistrLib
DEPENDPATH += $$PWD/../DistrLib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../build-DistrLib/release/libDistrLib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../build-DistrLib/debug/libDistrLib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../build-DistrLib/release/DistrLib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../build-DistrLib/debug/DistrLib.lib
else:unix: PRE_TARGETDEPS += $$PWD/../build-DistrLib/libDistrLib.a

INCLUDEPATH += ../boost
