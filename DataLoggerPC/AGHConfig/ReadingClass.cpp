#include "ReadingClass.h"
#include <iostream>
#include <cstring>

using namespace std;

const unsigned int ReadingClass::BUFFER_SIZE;

ReadingClass::ReadingClass(string nameOfFile, RawDataParser& dataParser) : dataParser(dataParser), data(nameOfFile.c_str(), ios_base::binary) {
    clear_buffer(BUFFER_SIZE);
}

unsigned int ReadingClass::reading_uint32(){
    data.read(buffer, 4);
    return dataParser.interpret_unsigned_int(buffer, 4);
}

unsigned int ReadingClass::reading_uint16(){
    data.read(buffer, 2);
    return dataParser.interpret_unsigned_int(buffer, 2);
}

unsigned int ReadingClass::reading_uint8(){
    data.read(buffer, 1);
    return dataParser.interpret_unsigned_int(buffer, 1);
}

int ReadingClass::reading_int16(){
    data.read(buffer, 2);
    return dataParser.interpret_signed_int(buffer, 2);
}

string ReadingClass::reading_string(unsigned int length){

    unsigned int bytesLeft = length+1;
	string retString;

	while (bytesLeft > 0){
        data.read(buffer, static_cast<streamsize>(min(bytesLeft, BUFFER_SIZE)));
        bytesLeft -= static_cast<unsigned int>(data.gcount());

        retString.append(buffer, static_cast<size_t>(data.gcount()));
	}
	
	return retString;
}

void ReadingClass::clear_buffer(size_t length){
    memset(buffer, 0, length);
}

void ReadingClass::reading_bytes(char* aBuffer, unsigned int bytesNumber){

    unsigned int bytesLeft = bytesNumber+1;
    unsigned int bytesRead = 0;

    while (bytesLeft > 0){
        data.read(aBuffer, static_cast<streamsize>(bytesLeft));
        bytesLeft -= static_cast<unsigned int>(data.gcount());
        bytesRead += static_cast<unsigned int>(data.gcount());
    }
}

bool ReadingClass::eof() const{
    return data.eof();
}

const RawDataParser& ReadingClass::get_dataParser() const {
    return const_cast<RawDataParser&>(dataParser);
}

ReadableFromBin::~ReadableFromBin(){ }

