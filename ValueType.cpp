#include "ValueType.h"
#include <iostream>

using namespace std;

#define		CONFIG_SIGNED_TYPE_flag	(1<<0)
#define		CONFIG_16_BIT_TYPE_flag	(1<<1)
#define		CONFIG_ON_OFF_TYPE_flag	(1<<2)
#define		CONFIG_FLAG_TYPE_flag  	(1<<3)
#define		CONFIG_CUSTOM_TYPE_flag	(1<<4)


ValueType::ValueType(ReadingClass& reading_class,int& dlc){
	uint8_t numbers_8;
	uint8_t feature = reading_class.reading_uint8(numbers_8);
	
	m_singed_type = checkingBits(feature, CONFIG_SIGNED_TYPE_flag);
	m_on_off_flag_type = checkingBits(feature, CONFIG_ON_OFF_TYPE_flag);
	m_flag_type = checkingBits(feature, CONFIG_FLAG_TYPE_flag);
	m_custom_type = checkingBits(feature, CONFIG_CUSTOM_TYPE_flag);	

	if ((feature & CONFIG_16_BIT_TYPE_flag) != 0){
		m_length_16_bit_type = true;
		dlc+=2;
	}
	else{
		m_length_16_bit_type = false;
		dlc++;
	}
}

bool ValueType::checkingBits(uint8_t feature, unsigned char bit){
	if ((feature & bit) != 0)
		return true;
	else
		return false;
}

void ValueType::print(){
	cout << "ValueType:\n";
	cout << "Signed: " << m_singed_type;
	cout << "\t16 bits: " << m_length_16_bit_type;
	cout << "\tOn: " << m_on_off_flag_type;
	cout << "\tFlag: " << m_flag_type;
	cout << "\tCustom type: " << m_custom_type << endl;
}

void ValueType::writeToBinary(ofstream& data){
	uint8_t number_to_write_to_file = 0;

	if (m_singed_type == true){
		number_to_write_to_file |= CONFIG_SIGNED_TYPE_flag;
	}
	if (m_length_16_bit_type == true){
		number_to_write_to_file |= CONFIG_16_BIT_TYPE_flag;
	}
	if (m_on_off_flag_type == true){
		number_to_write_to_file |= CONFIG_ON_OFF_TYPE_flag;
	}
	if (m_flag_type == true){
		number_to_write_to_file |= CONFIG_FLAG_TYPE_flag;
	}
	if (m_custom_type == true){
		number_to_write_to_file |= CONFIG_CUSTOM_TYPE_flag;
	}

	data.write(reinterpret_cast<char*>(&number_to_write_to_file),sizeof(number_to_write_to_file));
}
