#-------------------------------------------------
#
# Project created by QtCreator 2010-04-21T11:03:16
#
#-------------------------------------------------

TARGET = TimeScanMX
TEMPLATE = app

RESOURCES = qchartmx.qrc

SOURCES += main.cpp\
        qchartmx.cpp

INCLUDEPATH += /usr/include/qwt5/ \
	/usr/local/include/qwt5 \
	/usr/include/qwt-qt4/


HEADERS  += qchartmx.h

FORMS    += qchartmx.ui

LIBS += -lqwt -lqepicspv -lblitz -lpoptmx

target.files = $$[TARGET]
target.path = $$[INSTALLBASE]/bin
INSTALLS += target

