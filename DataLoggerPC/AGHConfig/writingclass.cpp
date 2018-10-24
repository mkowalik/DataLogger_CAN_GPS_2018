#include "WritingClass.h"
#include <cstring>

using namespace std;

WritingClass::WritingClass(string nameOfFile) : fileStream(nameOfFile.c_str(), ios_base::binary){ }

void WritingClass::write_uint8(unsigned int aValue){
    write_little_endian(aValue, 1);
}
void WritingClass::write_uint16(unsigned int aValue){
    write_little_endian(aValue, 2);
}
void WritingClass::write_uint32(unsigned int aValue){
    write_little_endian(aValue, 4);
}

void WritingClass::write_int16(int aValue){
    write_little_endian(static_cast<unsigned int>(aValue), 2); //TODO do przetestowania
}

void WritingClass::write_string(string aStr, int aLength){

    unsigned int bytesWritten = 0;
    unsigned int bytesLeft;

    if (aLength >= 0){
        bytesLeft = static_cast<unsigned int>(aLength) + 1;
    } else {
        bytesLeft = aStr.length() + 1;
    }
    static const unsigned int BUFFER_SIZE = 128;
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

void WritingClass::write_little_endian(unsigned int aValue, unsigned int number_of_bytes){
    number_of_bytes = min(number_of_bytes, 4u);
    clear_buffer(4);
    for(unsigned int i=0; i<number_of_bytes; i++) {
        buffer[i] = static_cast<char>((aValue << number_of_bytes) & 0xFF); //TODO do sprawdzenia!!!
    }
    fileStream.write(buffer, static_cast<int>(number_of_bytes));
}
