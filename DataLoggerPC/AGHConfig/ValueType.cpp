#include "ValueType.h"
#include "ReadingClass.h"

static const unsigned char CONFIG_SIGNED_TYPE_flag	(1<<0);
static const unsigned char CONFIG_16_BIT_TYPE_flag	(1<<1);
static const unsigned char CONFIG_ON_OFF_TYPE_flag	(1<<2);
static const unsigned char CONFIG_FLAG_TYPE_flag  	(1<<3);
static const unsigned char CONFIG_CUSTOM_TYPE_flag	(1<<4);
static const unsigned char CONFIG_BIG_ENDIAN_TYPE_flag	(1<<5);

ValueType::ValueType() : ValueType(0){
}

ValueType::ValueType(unsigned char aFeature){
	feature = aFeature;
}

ValueType::ValueType(bool setSigned, bool set16bit, bool setOnOffType, bool setFlagType, bool setCustomType, bool setBigEndianType){
    feature = 0;
    if (setSigned){
        feature |= CONFIG_SIGNED_TYPE_flag;
    }
    if (set16bit){
        feature |= CONFIG_16_BIT_TYPE_flag;
        if (setBigEndianType){
            feature |= CONFIG_BIG_ENDIAN_TYPE_flag;
        }
    }
    if (setOnOffType) {
        feature |= CONFIG_ON_OFF_TYPE_flag;
    } else if (setFlagType){
        feature |= CONFIG_FLAG_TYPE_flag;
    } else if (setCustomType){
        feature |= CONFIG_CUSTOM_TYPE_flag;
    }
}

unsigned int ValueType::channelDLC() const{
	if (is16BitLength()){
        return 2U;
	} 
    return 1U;
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

bool ValueType::isBigEndianType() const {
    return feature&CONFIG_BIG_ENDIAN_TYPE_flag;
}

unsigned char ValueType::getHexValue() const {
    return static_cast<unsigned char>(feature);
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
    if (isBigEndianType()){
        valueToWriteToFile |= CONFIG_BIG_ENDIAN_TYPE_flag;
    }

    writer.write_uint8(valueToWriteToFile);
}

void ValueType::read_from_bin(ReadingClass& reader){
    feature = reader.reading_uint8();
}
