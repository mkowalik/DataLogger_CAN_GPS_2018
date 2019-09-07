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

CONFIG += c++17 -o0 -g

SOURCES += \
    downloaddatasd_dialog.cpp \
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
    AGHData/raw_data_parser.cpp \
    convertfile_thread.cpp \
    downloadingprogress_dialog.cpp \
    AGHData/fixed_point.cpp \
    AGHData/single_can_frame_data.cpp \
    AGHData/single_gps_frame_data.cpp \
    configure_logger_sd_dialog.cpp \
    AGHData/can_channel_with_last_value.cpp

HEADERS += \
    downloaddatasd_dialog.h \
    mainwindow.h \
    newchannel_dialog.h \
    newframe_dialog.h \
    AGHConfig/Config.h \
    AGHConfig/ConfigChannel.h \
    AGHConfig/ConfigFrame.h \
    AGHConfig/ReadingClass.h \
    AGHConfig/ValueType.h \
    AGHConfig/WritingClass.h \
    AGHData/data_file_class.h \
    AGHData/raw_data_parser.h \
    convertfile_thread.h \
    downloadingprogress_dialog.h \
    AGHData/fixed_point.h \
    AGHData/single_can_frame_data.h \
    AGHData/single_gps_frame_data.h \
    configure_logger_sd_dialog.h \
    AGHData/can_channel_with_last_value.h

FORMS += \
    downloaddatasd_dialog.ui \
    mainwindow.ui \
    newchannel_dialog.ui \
    newframe_dialog.ui \
    downloadingprogress_dialog.ui \
    configure_logger_sd_dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    agh_racing_logo.png

RESOURCES += \
    images.qrc
