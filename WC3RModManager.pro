#-------------------------------------------------
#
# Project created by QtCreator 2018-09-04T07:18:19
#
#-------------------------------------------------
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WC3RModManager
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

RC_FILE = WC3RModManager.rc

SOURCES += \
    config.cpp \
    main.cpp \
    mainwindow.cpp \
    settings.cpp \
    thread.cpp \
    utils.cpp

HEADERS += \
    config.h \
    mainwindow.h \
    settings.h \
    thread.h \
    utils.h

FORMS += \
        mainwindow.ui \
    settings.ui \
    filestatus.ui \
    about.ui \
    splashscreen.ui

RESOURCES += \
    icons.qrc \
    img.qrc
