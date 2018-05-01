#pragma once
#include "ReadingClass.h"
#include <stdint.h>
#include <fstream>

class ValueType: public ReadingClass{
public:
	ValueType(){}
	ValueType(std::ifstream& data, int&dlc);
	bool checkingBits(uint8_t feature, unsigned char bit);
	void print();

private:
	bool m_singed_type;			
	bool m_length_16_bit_type;
	bool m_on_off_flag_type;
	bool m_flag_type;
	bool m_custom_type;

};
