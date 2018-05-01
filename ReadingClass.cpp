#include "ReadingClass.h"

#include <iostream>

using namespace std;

int ReadingClass::m_ptr_in_file = 0;

ReadingClass::ReadingClass(){
	//cout << "Konstruktor ReadingClass\n";
	//m_ptr_in_file = 0;
}

uint16_t ReadingClass::reading_uint16(uint16_t& numbers_16, ifstream& data){
	data.seekg(m_ptr_in_file);
	data.read(reinterpret_cast<char*>(&numbers_16), sizeof(numbers_16));
	m_ptr_in_file += sizeof(uint16_t);
	return numbers_16;
}

uint8_t ReadingClass::reading_uint8(uint8_t& numbers_8, ifstream& data){

	data.seekg(m_ptr_in_file);
	data.read(reinterpret_cast<char*>(&numbers_8), sizeof(numbers_8));
	m_ptr_in_file += sizeof(uint8_t);
	return numbers_8;
}

char* ReadingClass::reading_char(char* name, ifstream& data){
	data.seekg(m_ptr_in_file);
	data.read(reinterpret_cast<char*>(name), 20); 
	m_ptr_in_file += 21;
	return name;
}


