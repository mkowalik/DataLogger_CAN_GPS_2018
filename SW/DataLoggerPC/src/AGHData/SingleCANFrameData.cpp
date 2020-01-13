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

unsigned char SingleCANFrameData::getRawDataValue(unsigned int byteIndex) const {
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

unsigned long long SingleCANFrameData::getSignalValueRaw(const ConfigSignal* pSignal) const {

    if (pSignal->getParentFrame() != this->pConfigFrame){
        throw std::invalid_argument("Signal is not member of the frame.");
    }
    if (pSignal->getStartBit() + pSignal->getLengthBits() > (getFrameDLC() * 8)){
        throw std::invalid_argument("Signal with given position and length exeeds DLC of the frame.");
    }

    unsigned int bitsLeft       = pSignal->getLengthBits();
    unsigned int bitsShiftRaw   = pSignal->getStartBit() % 8;
    unsigned int bitIt          = pSignal->getStartBit();
    unsigned long long ret      = 0;

    if (pSignal->getValueType().isBigEndianType()){
        while (bitsLeft > 0) {
            unsigned char actualByte = rawPayloadData[(bitIt/8)];
            unsigned char nextByte = 0;
            if ((bitIt/8)+1 < getFrameDLC()) {
                nextByte = rawPayloadData[(bitIt/8)+1];
            }
            actualByte <<= bitsShiftRaw;
            actualByte |= (nextByte >> (8U - bitsShiftRaw));

            if (bitsLeft % 8 != 0){
                actualByte >>= (8U - (bitsLeft % 8));
                actualByte &= (0xFF >> (8U - (bitsLeft % 8)));
            }
            ret |= (static_cast<unsigned long long>(actualByte) << (bitsLeft - (((bitsLeft % 8U) == 0) ? 8U : (bitsLeft % 8U))) );
            bitIt += ((bitsLeft % 8U) == 0) ? 8U : (bitsLeft % 8U);
            bitsShiftRaw = (bitsShiftRaw + bitsLeft) % 8U;
            bitsLeft -= ((bitsLeft % 8U) == 0) ? 8U : (bitsLeft % 8U);
        }
    } else {
        while (bitsLeft > 0) {
            unsigned char actualByte = rawPayloadData[bitIt/8];
            unsigned char nextByte = 0;
            if ((bitIt/8)+1 < getFrameDLC()) {
                nextByte = rawPayloadData[(bitIt/8)+1];
            }
            actualByte <<= bitsShiftRaw;
            actualByte |= (nextByte >> (8U - bitsShiftRaw));

            if (bitsLeft < 8){
                actualByte >>= (8U - bitsLeft);
                actualByte &= (0xFF >> (8U - bitsLeft));
            }
            ret |= (static_cast<unsigned long long>(actualByte) << (((bitIt - pSignal->getStartBit())/8) * 8U));
            bitIt += min(8U, bitsLeft);
            bitsLeft -= min(8U, bitsLeft);
        }
    }
    return ret;
}

double SingleCANFrameData::getSignalValueTransformed(const ConfigSignal* pSignal) const {
   double ret = static_cast<double>(this->getSignalValueRaw(pSignal));
   ret += static_cast<double>(pSignal->getOffset());
   ret *= static_cast<double>(pSignal->getMultiplier());
   ret /= static_cast<double>(pSignal->getDivider());
   return ret;
}

long long SingleCANFrameData::getSignalValueTransformedLL(const ConfigSignal* pSignal) const {
   long long ret = static_cast<long long>(this->getSignalValueRaw(pSignal));
   ret += static_cast<double>(pSignal->getOffset());
   ret *= static_cast<double>(pSignal->getMultiplier());
   ret /= static_cast<double>(pSignal->getDivider());
   return ret;
}
