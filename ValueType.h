#ifndef VALUETYPE_H
#define VALUETYPE_H
#include "ReadingClass.h"
#include <stdint.h>
#include <fstream>

class ValueType{
public:
	ValueType(){}
	ValueType(ReadingClass& reading_class, int& dlc);
	bool checkingBits(uint8_t feature, unsigned char bit);
	void print();
	void writeToBinary(std::ofstream& data);

private:
	bool m_singed_type;			
	bool m_length_16_bit_type;
	bool m_on_off_flag_type;
	bool m_flag_type;
	bool m_custom_type;
};

#endif // VALUETYPE_H
