#include "DataFileClass.h"
#include "AGHUtils/RawDataParser.h"
#include "CSVWriter.h"
#include "CSVWriterEventMode.h"
#include "CsvWriterStaticFreq.h"
#include "CSVWriterFrameByFrame.h"
#include <cstring>

using namespace std;

//<----- DataFileClass private methods ----->//

void DataFileClass::freeMemory(){

    delete (this->pConfig);
    this->pConfig = nullptr;

    for (SingleCANFrameData* pElem : this->pCanDataVector){
        delete pElem;
    }
    this->pCanDataVector.clear();
    this->pCanDataVector.shrink_to_fit();

    for (SingleGPSFrameData* pElem : this->pGpsDataVector){
        delete pElem;
    }
    this->pGpsDataVector.clear();
    this->pGpsDataVector.shrink_to_fit();
}

//<----- DataFileClass public methods ----->//

DataFileClass::DataFileClass() : pConfig(nullptr), pCanDataVector(), pGpsDataVector() {
    this->startTime.tm_min   = 0;
    this->startTime.tm_sec   = 0;
    this->startTime.tm_min   = 0;
    this->startTime.tm_hour  = 0;
    this->startTime.tm_mday  = 0;
    this->startTime.tm_mon   = 0;
    this->startTime.tm_year  = 0;
    this->startTime.tm_wday  = 0;
    this->startTime.tm_yday  = 0;
    this->startTime.tm_isdst = 0;
}

const Config& DataFileClass::getConfig() const {
    return const_cast<Config&>(*pConfig);
}

const tm DataFileClass::getStartTime() const {
    return startTime;
}

void DataFileClass::readFromBin(ReadingClass& reader) {

    this->freeMemory();

    this->pConfig = new Config(reader);

    startTime.tm_year = static_cast<int>(reader.reading_uint16(RawDataParser::UseDefaultEndian));
    startTime.tm_mon  = static_cast<int>(reader.reading_uint8());
    startTime.tm_mday = static_cast<int>(reader.reading_uint8());
    startTime.tm_hour = static_cast<int>(reader.reading_uint8());
    startTime.tm_min  = static_cast<int>(reader.reading_uint8());
    startTime.tm_sec  = static_cast<int>(reader.reading_uint8());

    while(!reader.eof()){
        unsigned int msTime   = reader.reading_uint32(RawDataParser::UseDefaultEndian);
        unsigned int frameID  = reader.reading_uint16(RawDataParser::UseDefaultEndian);
        if (frameID == DataFileClass::GPS_DATA_ID){
            SingleGPSFrameData* pDataRow = new SingleGPSFrameData(msTime, reader.get_dataParser());
            pDataRow->readFromBin(reader);
            this->pGpsDataVector.push_back(pDataRow);
        } else {
            ConfigFrame* pConfigFrame = pConfig->getFrameWithId(frameID);
            SingleCANFrameData* pDataRow = new SingleCANFrameData(msTime, pConfigFrame, reader);
            pCanDataVector.push_back(pDataRow);
        }
    }
}

void DataFileClass::write_to_csv(WritableToCSV::FileTimingMode mode, WritingClass& writer, char decimalSeparator, bool writeOnlyChangedValues){

    CSVWriter* pCSVWriter = nullptr;

    switch (mode) {
    case WritableToCSV::FileTimingMode::EventMode:
        pCSVWriter = new CSVWriterEventMode(decimalSeparator, writeOnlyChangedValues, pConfig, writer);
        break;
    case WritableToCSV::FileTimingMode::StaticPeriod10HzMode:
        pCSVWriter = new CSVWriterStaticFreq(decimalSeparator, writeOnlyChangedValues, pConfig, writer, 100);
        break;
    case WritableToCSV::FileTimingMode::StaticPeriod100HzMode:
        pCSVWriter = new CSVWriterStaticFreq(decimalSeparator, writeOnlyChangedValues, pConfig, writer, 10);
        break;
    case WritableToCSV::FileTimingMode::StaticPeriod250HzMode:
        pCSVWriter = new CSVWriterStaticFreq(decimalSeparator, writeOnlyChangedValues, pConfig, writer, 4);
        break;
    case WritableToCSV::FileTimingMode::StaticPeriod500HzMode:
        pCSVWriter = new CSVWriterStaticFreq(decimalSeparator, writeOnlyChangedValues, pConfig, writer, 2);
        break;
    case WritableToCSV::FileTimingMode::StaticPeriod1000HzMode:
        pCSVWriter = new CSVWriterStaticFreq(decimalSeparator, writeOnlyChangedValues, pConfig, writer, 1);
        break;
    case WritableToCSV::FileTimingMode::FrameByFrameMode:
        pCSVWriter = new CSVWriterFrameByFrame(decimalSeparator, pConfig, writer);
        break;
    }

    pCSVWriter->writeToCSV(*this);
}

DataFileClass::~DataFileClass(){
    this->freeMemory();
}
