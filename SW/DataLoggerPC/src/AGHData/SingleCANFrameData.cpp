#include "SingleCANFrameData.h"

#include <array>

using namespace std;

SingleCANFrameData::SingleCANFrameData(unsigned int msTime, const ConfigFrame* frameConfig, ReadingClass &reader) : msTime{msTime}, rawPayloadData{}, pConfigFrame(frameConfig) {
    readFromBin(reader);
}

SingleCANFrameData::SingleCANFrameData(unsigned int msTime, const ConfigFrame* pConfigFrame, vector<unsigned char> rawPayloadData) : msTime{msTime}, rawPayloadData{}, pConfigFrame{pConfigFrame} {
    setRawPayloadData(rawPayloadData);
}

SingleCANFrameData::SingleCANFrameData(unsigned int msTime, const ConfigFrame* pConfigFrame, unsigned char* data, unsigned int dlc) : msTime{msTime}, rawPayloadData{}, pConfigFrame(pConfigFrame) {
    setRawPayloadData(data, dlc);
}

const ConfigFrame* SingleCANFrameData::getFrameConfig() const {
    return this->pConfigFrame;
}

unsigned int SingleCANFrameData::getMsTime() const {
    return msTime;
}

unsigned int SingleCANFrameData::getFrameID() const {
    return pConfigFrame->getFrameID();
}

unsigned int SingleCANFrameData::getFrameDLC() const {
    return static_cast<unsigned int>(rawPayloadData.size());
}

vector<unsigned char> SingleCANFrameData::getRawData() const {
    return rawPayloadData;
}

unsigned char SingleCANFrameData::getRawDataByte(unsigned int byteIndex) const {
    if (byteIndex >= getFrameDLC()){
        throw invalid_argument("byteIndex exeeds frame DLC.");
    }
    return rawPayloadData[byteIndex];
}

void SingleCANFrameData::readFromBin(ReadingClass &reader){

    unsigned int dlc = reader.reading_uint8();
    if (dlc > ConfigFrame::MAX_DLC_VALUE){
        throw std::invalid_argument("DLC of data exeeds max DLC of frame.");
    }
    rawPayloadData.clear();
    for (unsigned int i=0; i < dlc; i++){
        rawPayloadData.emplace_back(static_cast<unsigned char>(reader.reading_uint8()));
    }
}

void SingleCANFrameData::setRawPayloadData(vector<unsigned char> data){
    if (data.size() > ConfigFrame::MAX_DLC_VALUE){
        throw std::invalid_argument("DLC of data exeeds max DLC of frame.");
    }
    rawPayloadData.swap(data);
}

void SingleCANFrameData::setRawPayloadData(unsigned char* data, unsigned int dlc){
    if (dlc > ConfigFrame::MAX_DLC_VALUE){
        throw std::invalid_argument("DLC of data exeeds max DLC of frame.");
    }
    rawPayloadData.clear();
    for (unsigned int i=0; i<dlc; i++){
        rawPayloadData.emplace_back(data[i]);
    }
}

unsigned long SingleCANFrameData::getSignalValueRaw(const ConfigSignal* pSignal) const {

    return pSignal->getRawValueFromFramePayload(getRawData());

}
