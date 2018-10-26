#include "ReadingClass.h"
#include <iostream>
#include <cstring>

using namespace std;

const unsigned int ReadingClass::BUFFER_SIZE;

ReadingClass::ReadingClass(string name_of_file) : data(name_of_file.c_str(), ios_base::binary) {}

unsigned int ReadingClass::reading_uint32(){
    clear_buffer(4);
    data.read(buffer, 4);
    return parse_little_endian(buffer);
}

unsigned int ReadingClass::reading_uint16(){
    clear_buffer(4);
    data.read(buffer, 2);
    return parse_little_endian(buffer);
}

unsigned int ReadingClass::reading_uint8(){
    clear_buffer(4);
    data.read(buffer, 1);
    return parse_little_endian(buffer);
}

int ReadingClass::reading_int16(){ //TODO mocno przetestować!
    clear_buffer(4);
    data.read(buffer, 2);
    short* shortPtr = reinterpret_cast<short*>(buffer);
    return static_cast<int>(*shortPtr);
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

unsigned int ReadingClass::parse_little_endian(char* aNumberToParse){
    unsigned int retNumber = 0;
    retNumber |= static_cast<unsigned int>(aNumberToParse[0]);
    retNumber |= static_cast<unsigned int>(aNumberToParse[1] << 8);
    retNumber |= static_cast<unsigned int>(aNumberToParse[2] << 16);
    retNumber |= static_cast<unsigned int>(aNumberToParse[3] << 24);

    return retNumber;
}

void ReadingClass::clear_buffer(size_t length){
    memset(buffer, 0, length);
}
/*
void ReadingClass::whereIsPtr(){
	cout << "Pointer is " << data.tellg() << endl;
}
*/
