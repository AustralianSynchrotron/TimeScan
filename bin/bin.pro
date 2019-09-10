#-------------------------------------------------
#
# Project created by QtCreator 2011-09-02T17:05:39
#
#-------------------------------------------------


TARGET = TimeScanMX
TEMPLATE = app
QT += core gui widgets
CONFIG += qwt



SOURCES +=\
    TimeScanMX_main.cpp \
    TimeScanMX_mainwindow.cpp

HEADERS  += \
    TimeScanMX_mainwindow.h

INCLUDEPATH += ../lib

LIBS += -L../lib -lTimeScanMX -lpoptmx

FORMS    += \
    TimeScanMX_mainwindow.ui

RESOURCES += \
    TimeScanMX.qrc

icon.path = $$[INSTALLBASE]/share/icons
icon.files += TimeScanIcon.png

desktop.path = $$[INSTALLBASE]/share/applications
desktop.files += TimeScan.desktop

target.files = $$[TARGET]
target.path = $$INSTALLBASE/bin

INSTALLS += target icon desktop
