
#include "AGHData/DataFileClass.h"

#include "AGHData/CSVWriter.h"
#include "AGHData/CSVWriterEventMode.h"
#include "AGHData/CSVWriterStaticFreq.h"
#include "AGHData/CSVWriterFrameByFrame.h"

#include <cstring>
#include <memory>
#include <map>
#include <string>

using namespace std;

//<----- DataFileClass private methods ----->//

void DataFileClass::freeMemory(){

    delete (this->pConfig);
    this->pConfig = nullptr;

    for (DataRow pElem : this->dataRows){
        std::visit([](auto arg){delete arg;}, pElem);
    }
    dataRows.clear();
    dataRows.shrink_to_fit();
}

//<----- DataFileClass public methods ----->//

DataFileClass::DataFileClass() : pConfig(nullptr), dataRows()
{
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

    startTime.tm_year = static_cast<int>(reader.reading_uint16());
    startTime.tm_mon  = static_cast<int>(reader.reading_uint8());
    startTime.tm_mday = static_cast<int>(reader.reading_uint8());
    startTime.tm_hour = static_cast<int>(reader.reading_uint8());
    startTime.tm_min  = static_cast<int>(reader.reading_uint8());
    startTime.tm_sec  = static_cast<int>(reader.reading_uint8());

    unsigned int prevMsTime;
    unsigned int prevFrameID;

    while(!reader.eof()){
        unsigned int msTime   = reader.reading_uint32();
        unsigned int frameID  = reader.reading_uint16();
        try {
            if (frameID == DataFileClass::GPS_DATA_ID){
                SingleGPSFrameData* pGPSDataRow = new SingleGPSFrameData(msTime, reader);
                dataRows.push_back(DataRow(pGPSDataRow));
            } else if (frameID == DataFileClass::CAN_ERROR_ID){
                SingleCANErrorData* pCanErrorRow = new SingleCANErrorData(msTime, reader);
                dataRows.push_back(DataRow(pCanErrorRow));
            } else {
                ConfigFrame* pConfigFrame = pConfig->getFrameWithId(frameID);
                SingleCANFrameData* pCANDataRow = new SingleCANFrameData(msTime, pConfigFrame, reader);
                dataRows.push_back(DataRow(pCANDataRow));
            }
            prevFrameID = frameID;
            prevMsTime = msTime;
        }  catch (std::logic_error e) {
            msTime = 0;
            throw e;
        }
    }
}

/**
 * @brief DataFileClass::write_to_csv
 * @param mode
 * @param writer
 * @param decimalSeparator
 * @param writeOnlyChangedValues
 * @return Returns vector with warnings occured during conversion.
 */
std::map<std::string, unsigned int> DataFileClass::write_to_csv(WritableToCSV::FileTimingMode mode, WritingClass& writer, char decimalSeparator, bool writeOnlyChangedValues){

    std::unique_ptr<CSVWriter> pCSVWriter;

    switch (mode) {
    case WritableToCSV::FileTimingMode::EventMode:
        pCSVWriter = std::make_unique<CSVWriterEventMode>(decimalSeparator, writeOnlyChangedValues, pConfig, writer);
        break;
    case WritableToCSV::FileTimingMode::StaticPeriod10HzMode:
        pCSVWriter = std::make_unique<CSVWriterStaticFreq>(decimalSeparator, writeOnlyChangedValues, pConfig, writer, 100);
        break;
    case WritableToCSV::FileTimingMode::StaticPeriod100HzMode:
        pCSVWriter = std::make_unique<CSVWriterStaticFreq>(decimalSeparator, writeOnlyChangedValues, pConfig, writer, 10);
        break;
    case WritableToCSV::FileTimingMode::StaticPeriod250HzMode:
        pCSVWriter = std::make_unique<CSVWriterStaticFreq>(decimalSeparator, writeOnlyChangedValues, pConfig, writer, 4);
        break;
    case WritableToCSV::FileTimingMode::StaticPeriod500HzMode:
        pCSVWriter = std::make_unique<CSVWriterStaticFreq>(decimalSeparator, writeOnlyChangedValues, pConfig, writer, 2);
        break;
    case WritableToCSV::FileTimingMode::StaticPeriod1000HzMode:
        pCSVWriter = std::make_unique<CSVWriterStaticFreq>(decimalSeparator, writeOnlyChangedValues, pConfig, writer, 1);
        break;
    case WritableToCSV::FileTimingMode::FrameByFrameMode:
        pCSVWriter = std::make_unique<CSVWriterFrameByFrame>(decimalSeparator, pConfig, writer);
        break;
    }

    pCSVWriter->writeToCSV(*this);

    return pCSVWriter->getWarnings();
}

DataFileClass::~DataFileClass(){
    this->freeMemory();
}
