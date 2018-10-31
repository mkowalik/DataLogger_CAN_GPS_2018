#include "raw_data_parser.h"

#include <cmath>
#include <algorithm>
using namespace std;

RawDataParser::RawDataParser(EndianessMode mode) : mode(mode) {
}

int RawDataParser::interpret_signed_int(char* raw_data, unsigned int bytes) const {  //TODO mocno przetestować!

    bytes = min(bytes, 4u);

    int retNumber = 0;

    for (unsigned int i=0; i<bytes; i++){
        retNumber |= ((static_cast<unsigned int>(*raw_data)) << (i*8));
    }


    if (raw_data[0] < 0){
        for (unsigned int i=3; i>=bytes; i--){
            retNumber |= ((static_cast<unsigned int>(0xFF)) << (i*8));
        }
    }

    return retNumber;
}

unsigned int RawDataParser::interpret_unsigned_int(char* raw_data, unsigned int bytes) const {

    bytes = min(bytes, 4u);

    unsigned int retNumber = 0;

    for (unsigned int i=0; i<bytes; i++){
        retNumber |= ((static_cast<unsigned int>(*raw_data)) << (i*8));
    }

    return retNumber;

}
