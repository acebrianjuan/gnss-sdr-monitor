#-------------------------------------------------
#
# Project created by QtCreator 2018-07-13T21:17:58
#
#-------------------------------------------------

QT      += \
        core \
        gui \
        network \
        charts \
        printsupport \
        webenginewidgets \
        location \
        quickwidgets \
        quick

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gnss-sdr-monitor
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS    += \
        -lboost_serialization \
        -lboost_system

SOURCES += \
        main.cpp \
        main_window.cpp \
        channel_table_model.cpp \
        constellation_delegate.cpp \
        cn0_delegate.cpp \
        doppler_delegate.cpp \
        led_delegate.cpp \
        preferences_dialog.cpp \
        monitor_pvt_wrapper.cpp

HEADERS += \
        main_window.h \
        channel_table_model.h \
        constellation_delegate.h \
        cn0_delegate.h \
        doppler_delegate.h \
        led_delegate.h \
        preferences_dialog.h \
        monitor_pvt.h \
        monitor_pvt_wrapper.h

FORMS += \
        main_window.ui \
        preferences_dialog.ui

RESOURCES += \
        resources.qrc
