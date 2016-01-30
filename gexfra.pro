TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    event.c \
    event_manager.c \
    state_machine.c \
    timer_manager.c \
    timeout.c

HEADERS += \
    event.h \
    event_manager.h \
    state_machine.h \
    timer_manager.h \
    timeout.h
