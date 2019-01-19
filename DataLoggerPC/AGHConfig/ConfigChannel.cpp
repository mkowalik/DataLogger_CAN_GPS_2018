#include "ConfigChannel.h"
#include <iostream>
#include <cstdint>

static const unsigned int CHANNEL_NAME_LENGHT = 20;
static const unsigned int UNIT_LENGTH = 20;
static const unsigned int COMMENT_LENGTH = 20;

ConfigChannel::ConfigChannel() : valueType(0){

}

unsigned int ConfigChannel::get_DLC() const {
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
    multiplier = min(aMultiplier, static_cast<int>(INT16_MAX));
    multiplier = max(aMultiplier, static_cast<int>(INT16_MIN));
}

void ConfigChannel::set_divider(int aDivider){
    divider = min(aDivider, static_cast<int>(UINT16_MAX));
}

void ConfigChannel::set_offset(int aOffset){
    offset = max(INT16_MIN, min (aOffset, INT16_MAX));
}

void ConfigChannel::set_channelName(string aChannelName){
    if (aChannelName.length() > CHANNEL_NAME_LENGHT){
        aChannelName.resize(CHANNEL_NAME_LENGHT);
    }
    aChannelName.resize(aChannelName.find_first_of(static_cast<char>(0)) + 1);
    channelName = aChannelName;
}

void ConfigChannel::set_unitName(string aUnitName){
    if (aUnitName.length() > UNIT_LENGTH){
        aUnitName.resize(UNIT_LENGTH);
    }
    aUnitName.resize(aUnitName.find_first_of(static_cast<char>(0)) + 1);
    unitName = aUnitName;
}

void ConfigChannel::set_comment(string aComment){
    if (aComment.length() > COMMENT_LENGTH){
        aComment.resize(COMMENT_LENGTH);
    }
    aComment.resize(aComment.find_first_of(static_cast<char>(0)) + 1);
    comment = aComment;
}

void ConfigChannel::write_to_bin(WritingClass& writer){

    get_valueType().write_to_bin(writer);

    writer.write_int16(get_multiplier());
    writer.write_uint16(get_divider());
    writer.write_int16(get_offset());

    writer.write_string(get_channelName(), CHANNEL_NAME_LENGHT);
    writer.write_string(get_unitName(), UNIT_LENGTH);
    writer.write_string(get_comment(), COMMENT_LENGTH);
}


void ConfigChannel::read_from_bin(ReadingClass& reader){

    valueType.read_from_bin(reader);

    set_multiplier(reader.reading_int16());
    set_divider(reader.reading_uint16());
    set_offset(reader.reading_int16());

    set_channelName(reader.reading_string(CHANNEL_NAME_LENGHT));
    set_unitName(reader.reading_string(UNIT_LENGTH));
    set_comment(reader.reading_string(COMMENT_LENGTH));
}
