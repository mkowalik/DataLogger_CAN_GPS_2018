#include "ValueType.h"

void ValueType::setByteValue(unsigned char _byteValue)
{
    ByteValueType tmp;
    tmp.b = _byteValue;
    if (tmp.s._unused != 0){
        throw std::invalid_argument("Raw value of signal value type is incorrect. Some of unused bits set to 1.");
    }
    (byteValue.b) = _byteValue;
}

ValueType::ValueType(unsigned char _byteValue){
    setByteValue(_byteValue);
}

ValueType::ValueType(bool setSigned, bool setBigEndianType) : ValueType(0) {
    byteValue.s.isSigned = setSigned ? 1 : 0;
    byteValue.s.isBigEndian = setBigEndianType ? 1 : 0;
}

bool ValueType::isSignedType() const {
    return byteValue.s.isSigned;
}

bool ValueType::isBigEndianType() const {
    return byteValue.s.isBigEndian;
}

unsigned char ValueType::getByteValue() const {
    return byteValue.b;
}

void ValueType::writeToBin(WritingClass& writer){
    writer.write_uint8(byteValue.b);
}

void ValueType::readFromBin(ReadingClass& reader){
    setByteValue(static_cast<unsigned char>(reader.reading_uint8()));
}
