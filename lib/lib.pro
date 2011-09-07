#-------------------------------------------------
#
# Project created by QtCreator 2010-04-21T11:03:16
#
#-------------------------------------------------

TARGET = TimeScanMX
TEMPLATE = lib
CONFIG += qwt

RESOURCES =
SOURCES += \
    timescan.cpp
HEADERS  += \
    timescan.h
FORMS    += \
    timescan.ui

LIBS += -lqtpv -lblitz -lpoptmx


headers.files = $$HEADERS
headers.path = $$INSTALLBASE/include
INSTALLS += headers

target.files = $$[TARGET]
target.path = $$INSTALLBASE/lib
INSTALLS += target
