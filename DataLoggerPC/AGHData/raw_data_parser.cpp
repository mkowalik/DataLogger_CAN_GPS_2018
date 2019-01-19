#include "raw_data_parser.h"

#include <cmath>
#include <algorithm>
using namespace std;

RawDataParser::RawDataParser(EndianessMode rawDataMode) : rawDataMode(rawDataMode) {
}

int RawDataParser::interpret_signed_int(char* raw_data, int bytes) const {  //TODO mocno przetestować! //TODO dlaczego nie zalezy to od mode!!!

    bytes = min(bytes, static_cast<int>(sizeof(int)));

    int extra_shift = static_cast<int>(sizeof(int)) - bytes;

    int retNumber = 0;

    if (rawDataMode == LittleEndian){
        for (int i=0; i<bytes; i++){
            retNumber |= ((static_cast<unsigned char>(raw_data[i])) << ((i+extra_shift)*8));
        }
    }

    retNumber >>= (extra_shift*8);

    return retNumber;
}

unsigned int RawDataParser::interpret_unsigned_int(char* raw_data, int bytes) const {

    int size_of_uint = sizeof(unsigned int);
    bytes = min(bytes, size_of_uint);

    unsigned int retNumber = 0;

    if (rawDataMode == LittleEndian){
        for (int i=0; i<bytes; i++){
            retNumber |= ((static_cast<unsigned char>(raw_data[i])) << (i*8));
        }
    }

    return retNumber;
}

void RawDataParser::write_signed_int(int value, char* retBuffer, unsigned int bytes){
    write_unsigned_int(*(reinterpret_cast<unsigned int*>(&value)), retBuffer, bytes);
}

void RawDataParser::write_unsigned_int(unsigned int value, char* retBuffer, unsigned int bytes){
    bytes = min(bytes, 4u);
    for(unsigned int i=0; i<bytes; i++) {
        retBuffer[i] = static_cast<char>((value >> (i*8)) & 0xFF); //TODO do sprawdzenia!!!
    }
}
