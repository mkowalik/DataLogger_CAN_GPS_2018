#-------------------------------------------------
#
# Project created by QtCreator 2018-08-07T11:38:36
#
#-------------------------------------------------

QT       += core gui widgets charts

TARGET = DataLoggerPC
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

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    downloaddatasd_dialog.cpp \
    configureloggersd_dialog.cpp \
    newframe_dialog.cpp \
    newchannel_dialog.cpp

HEADERS += \
        mainwindow.h \
    downloaddatasd_dialog.h \
    configureloggersd_dialog.h \
    newframe_dialog.h \
    newchannel_dialog.h

FORMS += \
        mainwindow.ui \
    downloaddatasd_dialog.ui \
    configureloggersd_dialog.ui \
    newframe_dialog.ui \
    newchannel_dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    agh_racing_logo.png

RESOURCES += \
    images.qrc
