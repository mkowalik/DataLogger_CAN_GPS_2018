#include "raw_data_parser.h"

#include <cmath>
#include <algorithm>
using namespace std;

RawDataParser::RawDataParser(EndianessMode rawDataMode) : rawDataMode(rawDataMode) {
}

int RawDataParser::interpret_signed_int(char* raw_data, unsigned int bytes) const {  //TODO mocno przetestować! //TODO dlaczego nie zalezy to od mode!!!

    bytes = min(bytes, 4u);

    int retNumber = 0;

    for (unsigned int i=0; i<bytes; i++){
        retNumber |= ((static_cast<unsigned int>(*(raw_data+i))) << (i*8));
    }


    if (raw_data[0] < 0){
        for (unsigned int i=3; i>=bytes; i--){
            retNumber |= ((0xFF) << (i*8));
        }
    }

    return retNumber;
}

unsigned int RawDataParser::interpret_unsigned_int(char* raw_data, unsigned int bytes) const {

    bytes = min(bytes, 4u);

    unsigned int retNumber = 0;

    for (unsigned int i=0; i<bytes; i++){
        retNumber |= ((static_cast<unsigned int>(*(raw_data+i))) << (i*8));
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
