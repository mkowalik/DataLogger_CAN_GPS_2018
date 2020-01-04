#include "ValueType.h"
#include "AGHUtils/ReadingClass.h"

static const unsigned char CONFIG_16_BIT_TYPE_flag	(1<<1);
static const unsigned char CONFIG_ON_OFF_TYPE_flag	(1<<2);
static const unsigned char CONFIG_FLAG_TYPE_flag  	(1<<3);
static const unsigned char CONFIG_CUSTOM_TYPE_flag	(1<<4);
static const unsigned char CONFIG_BIG_ENDIAN_TYPE_flag	(1<<5);

ValueType::ValueType() : ValueType(0){
}

ValueType::ValueType(unsigned char _byteValue){
    (byteValue.b) = _byteValue;
}

ValueType::ValueType(bool setSigned, bool set16bit, bool setOnOffType, bool setFlagType, bool setCustomType, bool setBigEndianType){
    (byteValue.b) = 0;
    byteValue.s.isSigned = setSigned ? 1 : 0;
    if (set16bit){
        (byteValue.b) |= CONFIG_16_BIT_TYPE_flag;
    }
    if (setBigEndianType){
        (byteValue.b) |= CONFIG_BIG_ENDIAN_TYPE_flag;
    }
    if (setOnOffType) {
        (byteValue.b) |= CONFIG_ON_OFF_TYPE_flag;
    } else if (setFlagType){
        (byteValue.b) |= CONFIG_FLAG_TYPE_flag;
    } else if (setCustomType){
        (byteValue.b) |= CONFIG_CUSTOM_TYPE_flag;
    }
}

bool ValueType::isSignedType() const {
    return byteValue.s.isSigned;
}

bool ValueType::is16BitLength() const {
    return (byteValue.b)&CONFIG_16_BIT_TYPE_flag;
}

bool ValueType::isOnOffType() const{
    return (byteValue.b)&CONFIG_ON_OFF_TYPE_flag;
}

bool ValueType::isFlagType() const {
    return (byteValue.b)&CONFIG_FLAG_TYPE_flag;
}

bool ValueType::isCustomType() const {
    return (byteValue.b)&CONFIG_CUSTOM_TYPE_flag;
}

bool ValueType::isBigEndianType() const {
    return (byteValue.b)&CONFIG_BIG_ENDIAN_TYPE_flag;
}

unsigned char ValueType::getByteValue() const {
    return static_cast<unsigned char>((byteValue.b));
}

void ValueType::writeToBin(WritingClass& writer){
    writer.write_uint8(byteValue.b);
}

void ValueType::readFromBin(ReadingClass& reader){
    (byteValue.b) = static_cast<unsigned char>(reader.reading_uint8());
}
