# -------------------------------------------------
# Project created by QtCreator 2010-04-21T11:03:16
# -------------------------------------------------

TARGET = TimeScanMX
TEMPLATE = lib
CONFIG += qwt
QT += printsupport


SOURCES += timescan.cpp \
    script.cpp

HEADERS += timescan.h \
    script.h

FORMS += timescan.ui \
    script.ui

LIBS += -lqtpv \
    -lblitz \
    -lpoptmx

OTHER_FILES += \
    listOfSignals.txt

headers.files = $$HEADERS
headers.path = $$INSTALLBASE/include
INSTALLS += headers

target.files = $$[TARGET]
target.path = $$INSTALLBASE/lib
INSTALLS += target

config.files = $$OTHER_FILES
config.path = /../../../../../../../../../../../etc # This is how I force it to be installed in /etc
INSTALLS += config
