#include "AGHData/CSVWriterEventMode.h"

using namespace std;

CSVWriterEventMode::CSVWriterEventMode(char decimalSeparator, bool writeValuesForChangedSignalsOnly, const Config* pConfig, WritingClass& writer) :
    CSVSignalsWriter (decimalSeparator, pConfig, writer), writeValuesForChangedSignalsOnly(writeValuesForChangedSignalsOnly){

}

void CSVWriterEventMode::writeToCSV(const DataFileClass& dataFileClass){

    this->writeHeaderRow();

    vector<SingleCANFrameData*>::const_iterator canIt = dataFileClass.getCANData().cbegin();
    vector<SingleGPSFrameData*>::const_iterator gpsIt = dataFileClass.getGPSData().cbegin();

    vector<SingleCANFrameData*>::const_iterator nextCanIt = canIt;
    vector<SingleGPSFrameData*>::const_iterator nextGpsIt = gpsIt;

    map<const ConfigFrame*, const SingleCANFrameData*> actualCANFramesValues;
    SingleGPSFrameData* actualGPSValue = nullptr;

    unsigned int actualMsTime = 0;

    while (true){

        if (canIt == dataFileClass.getCANData().cend() && gpsIt == dataFileClass.getGPSData().cend()){
            break;
        }

        if (canIt != dataFileClass.getCANData().cend()) {
            if ( (gpsIt == dataFileClass.getGPSData().cend()) || ((*canIt)->getMsTime() <= (*gpsIt)->getMsTime()) ) {
                actualCANFramesValues[(*canIt)->getFrameConfig()] = *canIt;
                actualMsTime = (*canIt)->getMsTime();
                nextCanIt = canIt+1;
            }
        }

        if (gpsIt != dataFileClass.getGPSData().cend()){
            if ((canIt != dataFileClass.getCANData().cend() && ((*gpsIt)->getMsTime() <= (*canIt)->getMsTime())) ||
                (canIt == dataFileClass.getCANData().cend()) ) {
                actualGPSValue = *gpsIt;
                actualMsTime = (*gpsIt)->getMsTime();
                nextGpsIt = gpsIt+1;
            }
        }


        this->writeSingleRow(actualMsTime, actualCANFramesValues, actualGPSValue);

        canIt = nextCanIt;
        gpsIt = nextGpsIt;

        if (this->writeValuesForChangedSignalsOnly){
            actualCANFramesValues.clear();
            actualGPSValue = nullptr;
        }
    }
}

CSVWriterEventMode::~CSVWriterEventMode(){

}
