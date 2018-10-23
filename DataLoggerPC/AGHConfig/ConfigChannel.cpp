#include "ConfigChannel.h"
#include <iostream>
#include <cstdint>

static const unsigned int CHANNEL_NAME_LENGHT = 20;
static const unsigned int UNIT_LENGTH = 20;
static const unsigned int COMMENT_LENGTH = 20;

ConfigChannel::ConfigChannel() : valueType(0){

}

unsigned int ConfigChannel::get_DLC(){
	return valueType.channelDLC();
}

ValueType ConfigChannel::get_valueType(){
    return valueType;
}

unsigned int ConfigChannel::get_multiplier(){
	return multiplier;
}

unsigned int ConfigChannel::get_divider(){
	return divider;
}

int ConfigChannel::get_offset(){
	return offset;
}

string ConfigChannel::get_channelName(){
	return channelName;
}

string ConfigChannel::get_unitName(){
	return unitName;
}

string ConfigChannel::get_comment(){
	return comment;
}

void ConfigChannel::set_valueType(ValueType aValueType){
	valueType = aValueType;
}

void ConfigChannel::set_multiplier(unsigned int aMultiplier){
    multiplier = min(aMultiplier, static_cast<unsigned int>(UINT16_MAX));
}

void ConfigChannel::set_divider(unsigned int aDivider){
    divider = min(aDivider, static_cast<unsigned int>(UINT16_MAX));
}

void ConfigChannel::set_offset(int aOffset){
    offset = max(INT16_MIN, min (aOffset, INT16_MAX));
}

void ConfigChannel::set_channelName(string aChannelName){
    channelName = aChannelName;
    channelName.resize(CHANNEL_NAME_LENGHT);
}

void ConfigChannel::set_unitName(string aUnitName){
    unitName = aUnitName;
    unitName.resize(UNIT_LENGTH);
}

void ConfigChannel::set_comment(string aComment){
    comment = aComment;
    comment.resize(COMMENT_LENGTH);
}

void ConfigChannel::write_bin(WritingClass& writer){

    get_valueType().write_bin(writer);

    writer.write_uint16(get_multiplier());
    writer.write_uint16(get_divider());
    writer.write_int16(get_offset());

    writer.write_string(get_channelName(), CHANNEL_NAME_LENGHT);
    writer.write_string(get_unitName(), UNIT_LENGTH);
    writer.write_string(get_comment(), COMMENT_LENGTH);
}


void ConfigChannel::read_bin(ReadingClass& reader){

    valueType.read_bin(reader);

    set_multiplier(reader.reading_uint16());
    set_multiplier(reader.reading_uint16());
    set_offset(reader.reading_int16());

    set_channelName(reader.reading_string(CHANNEL_NAME_LENGHT));
    set_unitName(reader.reading_string(UNIT_LENGTH));
    set_comment(reader.reading_string(COMMENT_LENGTH));
}
