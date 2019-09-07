#include "single_can_frame_data.h"

#include <array>

using namespace std;

/***************      SingleChannelData       ***************/

SingleChannelData::SingleChannelData(const ConfigChannel& channel, int value) : channel(channel), value(value){
}

const ConfigChannel& SingleChannelData::get_channel() const{
    return channel;
}

int SingleChannelData::get_value_raw() const {
    return value;
}

double SingleChannelData::get_value_transformed() const {
   double ret = static_cast<double>(value);
   ret += static_cast<double>(this->get_channel().get_offset());
   ret *= static_cast<double>(this->get_channel().get_multiplier());
   ret /= static_cast<double>(this->get_channel().get_divider());
   return ret;
}

int SingleChannelData::get_value_transformed_int() const {
   int ret = value;
   ret += this->get_channel().get_offset();
   ret *= this->get_channel().get_multiplier();
   ret /= this->get_channel().get_divider();
   return ret;
}

SingleChannelData::operator double() const {
    return get_value_transformed();
}

SingleChannelData::operator int() const {
    return get_value_transformed_int();
}

/***************      DataRow       ***************/

SingleCANFrameData::SingleCANFrameData(unsigned int msTime, const ConfigFrame& frameConfig, const RawDataParser& dataParser) : msTime(msTime), rawData{0}, frameConfig(frameConfig), dataParser(dataParser) {
}

unsigned int SingleCANFrameData::getMsTime() const {
    return msTime;
}

const vector<SingleChannelData>& SingleCANFrameData::getData() const {
    return channelsData;
}

int SingleCANFrameData::getFrameID() const {
    return frameConfig.get_ID();
}

unsigned int SingleCANFrameData::getFrameDLC() const {
    return frameConfig.get_DLC();
}

unsigned char SingleCANFrameData::getRawDataValue(unsigned int byteIndex) const {
    if (byteIndex >= getFrameDLC()){
        throw invalid_argument("byteIndex exeeds frame DLC.");
    }
    return rawData[byteIndex];
}

void SingleCANFrameData::read_from_bin(ReadingClass &reader){

    for (unsigned int i=0; i < frameConfig.get_DLC(); i++){
        rawData[i] = static_cast<unsigned char>(reader.reading_uint8());
    }

    int i=0;
    for (ConfigFrame::const_iterator it = frameConfig.cbegin(); it != frameConfig.cend(); ++it){
        int value = 0;
        if (it->get_valueType().isSignedType()){
            value = dataParser.interpret_signed_int(rawData+i, it->get_DLC(), it->get_valueType().isBigEndianType() ? RawDataParser::BigEndian : RawDataParser::LittleEndian);
        } else {
            value = static_cast<int>(dataParser.interpret_unsigned_int(rawData+i, it->get_DLC(), it->get_valueType().isBigEndianType() ? RawDataParser::BigEndian : RawDataParser::LittleEndian));
        }
        channelsData.push_back(SingleChannelData(*it, value));
        i += it->get_DLC();
    }

}
