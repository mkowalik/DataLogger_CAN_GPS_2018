#include "raw_data_parser.h"

#include <cmath>
#include <algorithm>
#include <stdexcept>
using namespace std;

RawDataParser::RawDataParser(EndianessMode rawDataMode) : rawDataMode(rawDataMode) {
}

int RawDataParser::interpret_signed_int(char* raw_data, int bytes, EndianessMode endianessMode) const {  //TODO mocno przetestować! //TODO dlaczego nie zalezy to od mode!!!

    bytes = min(bytes, static_cast<int>(sizeof(int)));


    if (endianessMode == UseDefaultEndian){
        endianessMode = this->rawDataMode;
    }
    int retNumber = 0;

    if (endianessMode == LittleEndian){

        int extra_shift = static_cast<int>(sizeof(int)) - bytes;
        for (int i=0; i<bytes; i++){
            retNumber |= ((static_cast<unsigned char>(raw_data[i])) << ((i+extra_shift)*8));
        }
        retNumber >>= (extra_shift*8);
    } else if (endianessMode == BigEndian){
        throw std::invalid_argument("Not implemented exception");
    } else {
        throw std::invalid_argument("Not supported format");
    }


    return retNumber;
}

unsigned int RawDataParser::interpret_unsigned_int(char* raw_data, int bytes, EndianessMode endianessMode) const {

    int size_of_uint = sizeof(unsigned int);
    bytes = min(bytes, size_of_uint);

    if (endianessMode == UseDefaultEndian){
        endianessMode = this->rawDataMode;
    }
    unsigned int retNumber = 0;

    if (rawDataMode == LittleEndian) {
        for(int i=0; i<bytes; i++) {
            retNumber |= ((static_cast<unsigned char>(raw_data[i])) << (i*8));
        }
    } else if (endianessMode == BigEndian){
        for (int i=0; i<bytes; i++) {
            retNumber |= ((static_cast<unsigned char>(raw_data[bytes])) << ((bytes-i-1)*8));
        }
    } else {
        throw std::invalid_argument("Not supported format");
    }

    return retNumber;
}

void RawDataParser::write_signed_int(int value, char* retBuffer, unsigned int bytes, EndianessMode endianessMode){
    write_unsigned_int(*(reinterpret_cast<unsigned int*>(&value)), retBuffer, bytes, endianessMode);
}

void RawDataParser::write_unsigned_int(unsigned int value, char* retBuffer, unsigned int bytes, EndianessMode endianessMode){
    bytes = min(bytes, 4u);
    if (endianessMode == UseDefaultEndian){
        endianessMode = this->rawDataMode;
    }
    if (endianessMode == LittleEndian){
        for(unsigned int i=0; i<bytes; i++) {
            retBuffer[i] = static_cast<char>((value >> (i*8)) & 0xFF); //TODO do sprawdzenia!!!
        }
    } else if (endianessMode == BigEndian){
        for(unsigned int i=0; i<bytes; i++) {
            retBuffer[i] = static_cast<char>((value >> ((bytes-i-1)*8)) & 0xFF); //TODO do sprawdzenia!!!
        }
    } else {
        throw std::invalid_argument("Not supported format");
    }
}
