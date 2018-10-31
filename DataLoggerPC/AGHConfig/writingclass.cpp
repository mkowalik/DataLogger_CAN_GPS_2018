#include "WritingClass.h"
#include <cstring>

using namespace std;

const unsigned int WritingClass::BUFFER_SIZE;

WritingClass::WritingClass(string nameOfFile, RawDataParser& dataParser) : fileStream(nameOfFile.c_str(), ios_base::binary), dataParser(dataParser){ }

void WritingClass::write_uint8(unsigned int aValue){
    clear_buffer(4);
    dataParser.write_unsigned_int(aValue, buffer, 1);
    fileStream.write(buffer, 1);
}
void WritingClass::write_uint16(unsigned int aValue){
    clear_buffer(4);
    dataParser.write_unsigned_int(aValue, buffer, 4);
    fileStream.write(buffer, 2);
}
void WritingClass::write_uint32(unsigned int aValue){
    clear_buffer(4);
    dataParser.write_unsigned_int(aValue, buffer, 4);
    fileStream.write(buffer, 4);
}

void WritingClass::write_int16(int aValue){
    clear_buffer(4);
    dataParser.write_signed_int(aValue, buffer, 2);
    fileStream.write(buffer, 2);
}

void WritingClass::write_string(string aStr, int aLength){

    unsigned int bytesWritten = 0;
    unsigned int bytesLeft;

    if (aLength >= 0){
        bytesLeft = static_cast<unsigned int>(aLength) + 1;
    } else {
        bytesLeft = aStr.length() + 1;
    }

    while (bytesLeft > 0){
        clear_buffer(BUFFER_SIZE);
        unsigned int toCopy = min(bytesLeft, BUFFER_SIZE);
        memcpy(buffer, aStr.c_str() + bytesWritten, bytesLeft);
        fileStream.write(buffer, static_cast<streamsize>(toCopy));
        bytesLeft -= toCopy;
        bytesWritten += toCopy;
    }
}

void WritingClass::clear_buffer(size_t length){
    memset(buffer, 0, length);
}

WritableToBin::~WritableToBin(){ }

WritableToCSV::~WritableToCSV(){ }
