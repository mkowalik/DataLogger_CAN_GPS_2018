#include "ValueType.h"
#include "ReadingClass.h"

static const unsigned char CONFIG_SIGNED_TYPE_flag	(1<<0);
static const unsigned char CONFIG_16_BIT_TYPE_flag	(1<<1);
static const unsigned char CONFIG_ON_OFF_TYPE_flag	(1<<2);
static const unsigned char CONFIG_FLAG_TYPE_flag  	(1<<3);
static const unsigned char CONFIG_CUSTOM_TYPE_flag	(1<<4);


ValueType::ValueType(unsigned char aFeature){
	feature = aFeature;
}

unsigned int ValueType::channelDLC() const{
	if (is16BitLength()){
		return 2;
	} 
	return 1;
}

bool ValueType::isSignedType() const {
	return feature&CONFIG_SIGNED_TYPE_flag;
}

bool ValueType::is16BitLength() const {
	return feature&CONFIG_16_BIT_TYPE_flag;
}

bool ValueType::isOnOffType() const{
	return feature&CONFIG_ON_OFF_TYPE_flag;
}

bool ValueType::isFlagType() const {
	return feature&CONFIG_FLAG_TYPE_flag;
}

bool ValueType::isCustomType() const {
	return feature&CONFIG_CUSTOM_TYPE_flag;
}

void ValueType::write_to_bin(WritingClass& writer){

    unsigned int valueToWriteToFile = 0;

    if (isSignedType()){
        valueToWriteToFile |= CONFIG_SIGNED_TYPE_flag;
    }
    if (is16BitLength()){
        valueToWriteToFile |= CONFIG_16_BIT_TYPE_flag;
    }
    if (isOnOffType()){
        valueToWriteToFile |= CONFIG_ON_OFF_TYPE_flag;
    }
    if (isFlagType()){
        valueToWriteToFile |= CONFIG_FLAG_TYPE_flag;
    }
    if (isCustomType()){
        valueToWriteToFile |= CONFIG_CUSTOM_TYPE_flag;
    }

    writer.write_uint8(valueToWriteToFile);
}

void ValueType::read_from_bin(ReadingClass& reader){
    feature = reader.reading_uint8();
}
