#include "AGHData/CSVWriterEventMode.h"

using namespace std;

CSVWriterEventMode::CSVWriterEventMode(char decimalSeparator, bool writeValuesForChangedSignalsOnly, const Config* pConfig, WritingClass& writer) :
    CSVSignalsWriter (decimalSeparator, pConfig, writer), writeValuesForChangedSignalsOnly(writeValuesForChangedSignalsOnly){

}

void CSVWriterEventMode::writeToCSV(const DataFileClass& dataFileClass){

    this->writeHeaderRow();

    map<const ConfigFrame*, const SingleCANFrameData*> actualCANFramesValues;
    CANErrorCode actualCANError;

    for (const DataFileClass::DataRow& row : dataFileClass.getDataRows()){

        if (std::holds_alternative<const SingleCANFrameData*>(row)) {
            actualCANFramesValues[std::get<const SingleCANFrameData*>(row)->getFrameConfig()] = std::get<const SingleCANFrameData*>(row);
            this->writeSingleRow(std::get<const SingleCANFrameData*>(row)->getMsTime(), actualCANFramesValues, actualCANError, nullptr);

        } else if (std::holds_alternative<const SingleCANErrorData*>(row)) {
            actualCANError = std::get<const SingleCANErrorData*>(row)->getErrorCode();
            this->writeSingleRow(std::get<const SingleCANErrorData*>(row)->getMsTime(), actualCANFramesValues, actualCANError, nullptr);

        } else if (std::holds_alternative<const SingleGPSFrameData*>(row)) {
            this->writeSingleRow(std::get<const SingleGPSFrameData*>(row)->getMsTime(), actualCANFramesValues, actualCANError, std::get<const SingleGPSFrameData*>(row));
        }

        if (this->writeValuesForChangedSignalsOnly){
            actualCANFramesValues.clear();
        }
        actualCANError.clear();
    }
}

CSVWriterEventMode::~CSVWriterEventMode(){

}
