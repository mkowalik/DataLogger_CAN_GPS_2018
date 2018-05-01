#pragma once
#include <stdint.h>
#include <fstream>


class ReadingClass{
public:
	ReadingClass();
	uint16_t reading_uint16(uint16_t& numbers_16, std::ifstream& data);
	char* reading_char(char* name, std::ifstream& data);
	uint8_t reading_uint8(uint8_t& numbers_8, std::ifstream& data);
	static int m_ptr_in_file;

	
};
