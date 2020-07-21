#include "RawDataParser.h"

#include <cmath>
#include <algorithm>
#include <stdexcept>

using namespace std;

RawDataParser::RawDataParser(EndianessMode rawDataMode) : defaultEndianessMode(rawDataMode) {
}

int RawDataParser::interpret_signed_int(unsigned char *raw_data, unsigned int bytesNumber) const
{
    return interpret_signed_int(raw_data, bytesNumber, defaultEndianessMode);
}

unsigned int RawDataParser::interpret_unsigned_int(unsigned char *raw_data, unsigned int bytesNumber) const
{
    return interpret_unsigned_int(raw_data, bytesNumber, defaultEndianessMode);
}

unsigned long long RawDataParser::interpret_unsigned_long_long(unsigned char *raw_data, unsigned int bytesNumber) const
{
    return interpret_unsigned_long_long(raw_data, bytesNumber, defaultEndianessMode);
}

int RawDataParser::interpret_signed_int(unsigned char* raw_data, unsigned int bytesNumber, EndianessMode endianessMode) const {  //TODO mocno przetestować! //TODO dlaczego nie zalezy to od mode!!!

    bytesNumber = min(bytesNumber, static_cast<unsigned int>(sizeof(int)));

    int retNumber = 0;
    if (endianessMode == LittleEndian){
        unsigned int extra_shift = static_cast<unsigned int>(sizeof(int)) - bytesNumber;
        for (unsigned int i=0; i<bytesNumber; i++){
            retNumber |= (raw_data[i]) << ((i+extra_shift)*8U);
        }
        retNumber >>= (extra_shift*8);
    } else if (endianessMode == BigEndian){
        throw std::invalid_argument("Not implemented exception");
    } else {
        throw std::invalid_argument("Not supported format");
    }

    return retNumber;
}

unsigned int RawDataParser::interpret_unsigned_int(unsigned char* raw_data, unsigned int bytesNumber, EndianessMode endianessMode) const {

    bytesNumber = min(bytesNumber, static_cast<unsigned int>(sizeof(unsigned int)));

    unsigned int retNumber = 0;
    if (defaultEndianessMode == LittleEndian) {
        for(unsigned int i=0; i<bytesNumber; i++) {
            retNumber |= (static_cast<unsigned int>(raw_data[i])) << (i*8U);
        }
    } else if (endianessMode == BigEndian){
        for (unsigned int i=0; i<bytesNumber; i++) {
            retNumber |= ((static_cast<unsigned int>(raw_data[i])) << (bytesNumber-i-1)*8U);
        }
    } else {
        throw std::invalid_argument("Not supported format");
    }

    return retNumber;
}

unsigned long long RawDataParser::interpret_unsigned_long_long(unsigned char* raw_data, unsigned int bytesNumber, EndianessMode endianessMode) const {

    bytesNumber = min(bytesNumber, static_cast<unsigned int>(sizeof(unsigned long long)));

    unsigned long long retNumber = 0;
    if (defaultEndianessMode == LittleEndian) {
        for(unsigned int i=0; i<bytesNumber; i++) {
            retNumber |= (static_cast<unsigned long long>(raw_data[i])) << (i*8U);
        }
    } else if (endianessMode == BigEndian){
        for (unsigned int i=0; i<bytesNumber; i++) {
            retNumber |= ((static_cast<unsigned long long>(raw_data[i])) << (bytesNumber-i-1)*8U);
        }
    } else {
        throw std::invalid_argument("Not supported format");
    }

    return retNumber;
}

void RawDataParser::write_signed_int(int value, unsigned char *retBuffer, unsigned int bytesNumber)
{
    write_signed_int(value, retBuffer, bytesNumber, defaultEndianessMode);
}

void RawDataParser::write_unsigned_int(unsigned int value, unsigned char *retBuffer, unsigned int bytesNumber)
{
    write_unsigned_int(value, retBuffer, bytesNumber, defaultEndianessMode);
}

void RawDataParser::write_unsigned_long_long(unsigned long long value, unsigned char *retBuffer, unsigned int bytes)
{
    write_unsigned_long_long(value, retBuffer, bytes, defaultEndianessMode);
}

void RawDataParser::write_signed_int(int value, unsigned char* retBuffer, unsigned int bytes, EndianessMode endianessMode){
    write_unsigned_int(*(reinterpret_cast<unsigned int*>(&value)), retBuffer, bytes, endianessMode);
}

void RawDataParser::write_unsigned_int(unsigned int value, unsigned char* retBuffer, unsigned int bytes, EndianessMode endianessMode){
    bytes = min(bytes, 4u);
    if (endianessMode == LittleEndian){
        for(unsigned int i=0; i<bytes; i++) {
            retBuffer[i] = static_cast<unsigned char>((value >> (i*8)) & 0xFF); //TODO do sprawdzenia!!!
        }
    } else if (endianessMode == BigEndian){
        for(unsigned int i=0; i<bytes; i++) {
            retBuffer[i] = static_cast<unsigned char>((value >> ((bytes-i-1)*8)) & 0xFF); //TODO do sprawdzenia!!!
        }
    } else {
        throw std::invalid_argument("Not supported format");
    }
}

void RawDataParser::write_unsigned_long_long(unsigned long long value, unsigned char* retBuffer, unsigned int bytes, EndianessMode endianessMode){
    bytes = min(bytes, 8u);
    if (endianessMode == LittleEndian){
        for(unsigned int i=0; i<bytes; i++) {
            retBuffer[i] = static_cast<unsigned char>((value >> (i*8)) & 0xFF); //TODO do sprawdzenia!!!
        }
    } else if (endianessMode == BigEndian){
        for(unsigned int i=0; i<bytes; i++) {
            retBuffer[i] = static_cast<unsigned char>((value >> ((bytes-i-1)*8)) & 0xFF); //TODO do sprawdzenia!!!
        }
    } else {
        throw std::invalid_argument("Not supported format");
    }
}

RawDataParser::EndianessMode RawDataParser::getDefaultEndianessMode()
{
    return defaultEndianessMode;
}
