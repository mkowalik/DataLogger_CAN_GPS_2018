#include "ConfigChannel.h"
#include <iostream>
#include <cstdint>

static const unsigned int CHANNEL_NAME_LENGHT = 20;
static const unsigned int UNIT_LENGTH = 20;
static const unsigned int COMMENT_LENGTH = 20;

ConfigChannel::ConfigChannel() : valueType(0) {
}

int ConfigChannel::get_DLC() const {
	return valueType.channelDLC();
}

ValueType ConfigChannel::get_valueType() const {
    return valueType;
}

int ConfigChannel::get_multiplier() const {
	return multiplier;
}

int ConfigChannel::get_divider() const {
	return divider;
}

int ConfigChannel::get_offset() const {
	return offset;
}

string ConfigChannel::get_channelName() const {
	return channelName;
}

string ConfigChannel::get_unitName() const {
	return unitName;
}

string ConfigChannel::get_comment() const {
	return comment;
}

void ConfigChannel::set_valueType(ValueType aValueType){
	valueType = aValueType;
}

void ConfigChannel::set_multiplier(int aMultiplier){
    if ((aMultiplier < INT16_MIN) || (aMultiplier > INT16_MAX)){
        std::invalid_argument("Multiplier should be between INT16_MIN and INT16_MAX");
    }
    multiplier = aMultiplier;
}

void ConfigChannel::set_divider(int aDivider){
    if ((aDivider < 1) || (aDivider > UINT16_MAX)){
        std::invalid_argument("Divider should be between 1 and UINT16_MAX");
    }
    divider = aDivider;
}

void ConfigChannel::set_offset(int aOffset){
    if ((aOffset < INT16_MIN) || (aOffset > INT16_MAX)){
        std::invalid_argument("Offset should be between INT16_MIN and INT16_MAX");
    }
    offset = aOffset;
}

void ConfigChannel::set_channelName(string aChannelName){
    if (aChannelName.length() > CHANNEL_NAME_LENGHT){
        aChannelName.resize(CHANNEL_NAME_LENGHT);
    }
    if (aChannelName.find_first_of(static_cast<char>(0)) != string::npos){
        aChannelName.resize(aChannelName.find_first_of(static_cast<char>(0)));
    }
    channelName = aChannelName;
}

void ConfigChannel::set_unitName(string aUnitName){
    if (aUnitName.length() > UNIT_LENGTH){
        aUnitName.resize(UNIT_LENGTH);
    }
    if (aUnitName.find_first_of(static_cast<char>(0)) != string::npos){
        aUnitName.resize(aUnitName.find_first_of(static_cast<char>(0)));
    }
    unitName = aUnitName;
}

void ConfigChannel::set_comment(string aComment){
    if (aComment.length() > COMMENT_LENGTH){
        aComment.resize(COMMENT_LENGTH);
    }
    if (aComment.find_first_of(static_cast<char>(0)) != string::npos){
        aComment.resize(aComment.find_first_of(static_cast<char>(0)));
    }
    comment = aComment;
}

void ConfigChannel::write_to_bin(WritingClass& writer){

    get_valueType().write_to_bin(writer);

    writer.write_int16(get_multiplier(), RawDataParser::UseDefaultEndian);
    writer.write_uint16(get_divider(), RawDataParser::UseDefaultEndian);
    writer.write_int16(get_offset(), RawDataParser::UseDefaultEndian);

    writer.write_string(get_channelName(), true, CHANNEL_NAME_LENGHT);
    writer.write_string(get_unitName(), true, UNIT_LENGTH);
    writer.write_string(get_comment(), true, COMMENT_LENGTH);
}


void ConfigChannel::read_from_bin(ReadingClass& reader){

    valueType.read_from_bin(reader);

    set_multiplier(reader.reading_int16(RawDataParser::UseDefaultEndian));
    set_divider(reader.reading_uint16(RawDataParser::UseDefaultEndian));
    set_offset(reader.reading_int16(RawDataParser::UseDefaultEndian));

    set_channelName(reader.reading_string(CHANNEL_NAME_LENGHT, true));
    set_unitName(reader.reading_string(UNIT_LENGTH, true));
    set_comment(reader.reading_string(COMMENT_LENGTH, true));
}
