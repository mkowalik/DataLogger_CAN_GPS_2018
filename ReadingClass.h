#ifndef READINGCLASS_H
#define READINGCLASS_H
#include <stdint.h>
#include <fstream>


class ReadingClass{
public:
	ReadingClass(std::string name_of_file);
	uint16_t reading_uint16(uint16_t& numbers_16);
	char* reading_char(char* name);
	uint8_t reading_uint8(uint8_t& numbers_8);
	void whereIsPtr();
private:
	std::ifstream data;
};

#endif // READINGCLASS_H
