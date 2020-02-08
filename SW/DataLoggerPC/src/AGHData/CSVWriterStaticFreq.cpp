#include "CSVWriterStaticFreq.h"

CSVWriterStaticFreq::CSVWriterStaticFreq(char decimalSeparator, bool writeValuesForChangedSignalsOnly, const Config* pConfig, WritingClass& writer, unsigned int periodMs) :
    CSVSignalsWriter (decimalSeparator, pConfig, writer), writeValuesForChangedSignalsOnly(writeValuesForChangedSignalsOnly), periodMs(periodMs) {

}

void CSVWriterStaticFreq::writeToCSV(const DataFileClass& dataFileClass){

    this->writeHeaderRow();

    map<const ConfigFrame*, const SingleCANFrameData*> actualCANFramesValues;
    CANErrorCode actualCANError;
    const SingleGPSFrameData* actualGPSValue = nullptr;

    unsigned int actualMsTime = periodMs;

    for (auto it = dataFileClass.getDataRows().cbegin(); it < dataFileClass.getDataRows().cend(); it++){

        if (std::holds_alternative<const SingleCANFrameData*>(*it)) {
            actualCANFramesValues[std::get<const SingleCANFrameData*>(*it)->getFrameConfig()] = std::get<const SingleCANFrameData*>(*it);

        } else if (std::holds_alternative<const SingleCANErrorData*>(*it)) {
            actualCANError.mergeWithOther(std::get<const SingleCANErrorData*>(*it)->getErrorCode());

        } else if (std::holds_alternative<const SingleGPSFrameData*>(*it)) {
            actualGPSValue = std::get<const SingleGPSFrameData*>(*it);
        }

        if ((it+1 == dataFileClass.getDataRows().cend()) || (std::visit([&](auto pRow){return ((pRow->getMsTime()) > (actualMsTime+periodMs));}, *it) )){
            this->writeSingleRow(actualMsTime, actualCANFramesValues, actualCANError, actualGPSValue);
            actualMsTime += this->periodMs;
            if (writeValuesForChangedSignalsOnly){
                actualCANFramesValues.clear();
                actualGPSValue = nullptr;
            }
            actualCANError.clear();
        }
    }
}

CSVWriterStaticFreq::~CSVWriterStaticFreq(){

}
