include(../defaults.pri)

QT += testlib
CONFIG += core widgets gui qt warn_on depend_includepath testcase
CONFIG += c++17 -o0 -g

TEMPLATE = app

SOURCES +=  tst_singlecanframedata.cpp \
    ../src/AGHConfig/Config.cpp \
    ../src/AGHConfig/ConfigFrame.cpp \
    ../src/AGHConfig/ConfigSignal.cpp \
    ../src/AGHConfig/ValueType.cpp \
    ../src/AGHConfig/ConfigTrigger.cpp \
    ../src/AGHData/CSVSignalsWriter.cpp \
    ../src/AGHData/CSVWriterEventMode.cpp \
    ../src/AGHData/CSVWriterFrameByFrame.cpp \
    ../src/AGHData/CSVWriterStaticFreq.cpp \
    ../src/AGHData/DataFileClass.cpp \
    ../src/AGHData/SingleCANFrameData.cpp \
    ../src/AGHData/SingleGPSFrameData.cpp \
    ../src/AGHUtils/FixedPoint.cpp \
    ../src/AGHUtils/RawDataParser.cpp \
    ../src/AGHUtils/ReadingClass.cpp \
    ../src/AGHUtils/WritingClass.cpp

HEADERS += \
    ../src/AGHConfig/Config.h \
    ../src/AGHConfig/ConfigFrame.h \
    ../src/AGHConfig/ConfigSignal.h \
    ../src/AGHConfig/ValueType.h \
    ../src/AGHConfig/ConfigTrigger.h \
    ../src/AGHData/CSVSignalsWriter.h \
    ../src/AGHData/CSVWriter.h \
    ../src/AGHData/CSVWriterEventMode.h \
    ../src/AGHData/CSVWriterFrameByFrame.h \
    ../src/AGHData/CSVWriterStaticFreq.h \
    ../src/AGHData/DataFileClass.h \
    ../src/AGHData/SingleCANFrameData.h \
    ../src/AGHData/SingleGPSFrameData.h \
    ../src/AGHData/WritableToCSV.h \
    ../src/AGHUtils/FixedPoint.h \
    ../src/AGHUtils/RawDataParser.h \
    ../src/AGHUtils/ReadingClass.h \
    ../src/AGHUtils/WritingClass.h


qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
