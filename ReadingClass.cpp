#include "ReadingClass.h"
#include <iostream>

using namespace std;

ReadingClass::ReadingClass(string name_of_file): data(name_of_file.c_str(), ios_base::binary) {}

uint16_t ReadingClass::reading_uint16(uint16_t& numbers_16){
	data.read(reinterpret_cast<char*>(&numbers_16), sizeof(numbers_16));
	return numbers_16;
}

uint8_t ReadingClass::reading_uint8(uint8_t& numbers_8){
	data.read(reinterpret_cast<char*>(&numbers_8), sizeof(numbers_8));
	return numbers_8;
}

char* ReadingClass::reading_char(char* name){
	data.read(reinterpret_cast<char*>(name), 21); 
	return name;
}

void ReadingClass::whereIsPtr(){
	cout << "Pointer is " << data.tellg() << endl;
}
