#-------------------------------------------------
#
# Project created by QtCreator 2018-08-07T11:38:36
#
#-------------------------------------------------

include (../defaults.pri)

QT       += core gui widgets charts

TARGET = src
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
    AGHConfig/Config.cpp \
    AGHConfig/ConfigFrame.cpp \
    AGHConfig/ConfigSignal.cpp \
    AGHConfig/ValueType.cpp \
    AGHData/CSVSignalsWriter.cpp \
    AGHData/CSVWriterEventMode.cpp \
    AGHData/CSVWriterFrameByFrame.cpp \
    AGHData/CSVWriterStaticFreq.cpp \
    AGHData/DataFileClass.cpp \
    AGHData/SingleCANFrameData.cpp \
    AGHData/SingleGPSFrameData.cpp \
    AGHUtils/FixedPoint.cpp \
    AGHUtils/RawDataParser.cpp \
    AGHUtils/ReadingClass.cpp \
    AGHUtils/WritingClass.cpp \
    configure_logger_sd_dialog.cpp \
    convertfile_thread.cpp \
    downloaddatasd_dialog.cpp \
    downloadingprogress_dialog.cpp \
    main.cpp \
    mainwindow.cpp \
    newchannel_dialog.cpp \
    newframe_dialog.cpp


HEADERS += \
    AGHConfig/Config.h \
    AGHConfig/ConfigFrame.h \
    AGHConfig/ConfigSignal.h \
    AGHConfig/ValueType.h \
    AGHData/CSVSignalsWriter.h \
    AGHData/CSVWriter.h \
    AGHData/CSVWriterEventMode.h \
    AGHData/CSVWriterFrameByFrame.h \
    AGHData/CSVWriterStaticFreq.h \
    AGHData/DataFileClass.h \
    AGHData/SingleCANFrameData.h \
    AGHData/SingleGPSFrameData.h \
    AGHData/WritableToCSV.h \
    AGHUtils/FixedPoint.h \
    AGHUtils/RawDataParser.h \
    AGHUtils/ReadingClass.h \
    AGHUtils/WritingClass.h \
    configure_logger_sd_dialog.h \
    convertfile_thread.h \
    downloaddatasd_dialog.h \
    downloadingprogress_dialog.h \
    mainwindow.h \
    newchannel_dialog.h \
    newframe_dialog.h

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
