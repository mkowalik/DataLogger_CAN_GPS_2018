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
    configureloggersd_dialog.cpp \
    downloaddatasd_dialog.cpp \
    downloading_dialog.cpp \
    main.cpp \
    mainwindow.cpp \
    newchannel_dialog.cpp \
    newframe_dialog.cpp \
    AGHConfig/Config.cpp \
    AGHConfig/ConfigChannel.cpp \
    AGHConfig/ConfigFrame.cpp \
    AGHConfig/ReadingClass.cpp \
    AGHConfig/ValueType.cpp \
    AGHConfig/WritingClass.cpp \
    AGHData/data_file_class.cpp \
    AGHData/raw_data_parser.cpp

HEADERS += \
    configureloggersd_dialog.h \
    downloaddatasd_dialog.h \
    downloading_dialog.h \
    mainwindow.h \
    newchannel_dialog.h \
    newframe_dialog.h \
    ui_configureloggersd_dialog.h \
    ui_downloaddatasd_dialog.h \
    ui_mainwindow.h \
    ui_newchannel_dialog.h \
    ui_newframe_dialog.h \
    AGHConfig/Config.h \
    AGHConfig/ConfigChannel.h \
    AGHConfig/ConfigFrame.h \
    AGHConfig/ReadingClass.h \
    AGHConfig/ValueType.h \
    AGHConfig/WritingClass.h \
    AGHData/data_file_class.h \
    AGHData/raw_data_parser.h

FORMS += \
        mainwindow.ui \
    downloaddatasd_dialog.ui \
    configureloggersd_dialog.ui \
    newframe_dialog.ui \
    newchannel_dialog.ui \
    downloading_dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    agh_racing_logo.png

RESOURCES += \
    images.qrc
