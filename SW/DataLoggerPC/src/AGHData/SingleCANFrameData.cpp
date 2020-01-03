#include "SingleCANFrameData.h"

#include <array>

using namespace std;

SingleCANFrameData::SingleCANFrameData(unsigned int msTime, const ConfigFrame* frameConfig) : msTime(msTime), rawData{0}, pConfigFrame(frameConfig) {
}

const ConfigFrame* SingleCANFrameData::getFrameConfig() const {
    return this->pConfigFrame;
}

unsigned int SingleCANFrameData::getMsTime() const {
    return msTime;
}

unsigned int SingleCANFrameData::getFrameID() const {
    return pConfigFrame->getID();
}

unsigned int SingleCANFrameData::getFrameDLC() const {
    return pConfigFrame->getDLC();
}

unsigned char SingleCANFrameData::getRawDataValue(unsigned int byteIndex) const {
    if (byteIndex >= getFrameDLC()){
        throw invalid_argument("byteIndex exeeds frame DLC.");
    }
    return rawData[byteIndex];
}

void SingleCANFrameData::readFromBin(ReadingClass &reader){

    for (unsigned int i=0; i < pConfigFrame->getDLC(); i++){
        rawData[i] = static_cast<unsigned char>(reader.reading_uint8());
    }

}


void SingleCANFrameData::setRawData(unsigned char* data){

    for (unsigned int i=0; i<this->getFrameDLC(); i++){
        this->rawData[i] = data[i];
    }

}

/*bool SingleCANFrameData::equalIDAndPayload(const SingleCANFrameData& b) const {

    if (this->getFrameID() != b.getFrameID()){
        return false;
    }

    if (this->getFrameDLC() != b.getFrameDLC()){
        return false;
    }

    for (unsigned int i = 0; i < this->getFrameDLC(); i++){
        if (this->getRawDataValue(i) != b.getRawDataValue(i)){
            return false;
        }
    }
    return true;
}*/

unsigned long long SingleCANFrameData::getSignalValueRaw(const ConfigSignal* pSignal) const {

    if (!(pSignal->getParentFrame() == this->pConfigFrame)){
        throw std::invalid_argument("Signal is not member of the frame.");
    }

    unsigned int bitsLeft       = pSignal->getLengthBits();
    unsigned int bitsShiftRaw   = pSignal->getStartBit() % 8;
    unsigned int bitIt          = pSignal->getStartBit();
    unsigned long long ret = 0;

    if (pSignal->getValueType().isBigEndianType()){
        while (bitsLeft > 0) {
            unsigned char actualByte = rawData[(bitIt/8)];
            unsigned char nextByte = 0;
            if ((bitIt/8)+1 < pSignal->getParentFrame()->getDLC()) {
                nextByte = rawData[(bitIt/8)+1];
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
            unsigned char actualByte = rawData[bitIt/8];
            unsigned char nextByte = 0;
            if ((bitIt/8)+1 < pSignal->getParentFrame()->getDLC()) {
                nextByte = rawData[(bitIt/8)+1];
            }
            actualByte <<= bitsShiftRaw;
            actualByte |= (nextByte >> (8U - bitsShiftRaw));

            if (bitsLeft < 8){
                actualByte >>= (8U - bitsLeft);
                actualByte &= (0xFF >> (8U - bitsLeft));
            }
            ret |= (static_cast<unsigned long long>(actualByte) << (((bitIt - pSignal->getStartBit())/8) * 8U)); //Tu raczej blad
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
