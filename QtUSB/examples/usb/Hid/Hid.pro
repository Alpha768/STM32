#-------------------------------------------------
#
# Project created by QtCreator 2015-07-28T13:55:22
#
#-------------------------------------------------

QT += core usb
QT -= gui

TARGET = Hid
TEMPLATE = app

SOURCES += main.cpp \
    hidexample.cpp

HEADERS += \
    hidexample.h

target.path = $$[QT_INSTALL_EXAMPLES]/usb/Hid
INSTALLS += target
