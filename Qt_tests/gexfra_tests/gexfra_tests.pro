QT += core
QT -= gui

CONFIG += c++11

TARGET = gexfra_tests
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    ../../gexfra.c \
    timerslot.cpp

HEADERS += \
    ../../defines.h \
    ../../gexfra.h \
    timerslot.h
