#include "CSVWriterStaticFreq.h"

CSVWriterStaticFreq::CSVWriterStaticFreq(char decimalSeparator, bool writeValuesForChangedSignalsOnly, const Config* pConfig, WritingClass& writer, unsigned int periodMs) :
    CSVSignalsWriter (decimalSeparator, pConfig, writer), writeValuesForChangedSignalsOnly(writeValuesForChangedSignalsOnly), periodMs(periodMs) {

}

void CSVWriterStaticFreq::writeToCSV(const DataFileClass& dataFileClass){

    this->writeHeaderRow();

    vector<SingleCANFrameData*>::const_iterator canIt = dataFileClass.getCANData().cbegin();
    vector<SingleGPSFrameData*>::const_iterator gpsIt = dataFileClass.getGPSData().cbegin();

    vector<SingleCANFrameData*>::const_iterator nextCanIt = canIt;
    vector<SingleGPSFrameData*>::const_iterator nextGpsIt = gpsIt;

    map<const ConfigFrame*, const SingleCANFrameData*> actualCANFramesValues;
    SingleGPSFrameData* actualGPSValue = nullptr;

    unsigned int actualMsTime = periodMs;
    unsigned int lastReadMsTime = 0;

    while (true){

        if (canIt == dataFileClass.getCANData().cend() && gpsIt == dataFileClass.getGPSData().cend()){
            break;
        }

        if (canIt != dataFileClass.getCANData().cend()) {
            if ((*canIt)->getMsTime() <= actualMsTime) {
                actualCANFramesValues[(*canIt)->getFrameConfig()] = *canIt;
                nextCanIt = canIt+1;
                lastReadMsTime = (*canIt)->getMsTime();
            }
        }

        if (gpsIt != dataFileClass.getGPSData().cend()){
            if ((*gpsIt)->getMsTime() <= actualMsTime) {
                actualGPSValue = *gpsIt;
                nextGpsIt = gpsIt+1;
                lastReadMsTime = max((*gpsIt)->getMsTime(), lastReadMsTime);
            }
        }

        if (lastReadMsTime == 0){
            this->writeSingleRow(actualMsTime, actualCANFramesValues, actualGPSValue);
            actualMsTime += this->periodMs;
            if (writeValuesForChangedSignalsOnly){
                actualCANFramesValues.clear();
                actualGPSValue = nullptr;
            }
        }

        canIt           = nextCanIt;
        gpsIt           = nextGpsIt;
        lastReadMsTime  = 0;
    }
}

CSVWriterStaticFreq::~CSVWriterStaticFreq(){

}
