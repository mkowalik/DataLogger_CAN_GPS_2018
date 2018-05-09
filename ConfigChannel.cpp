#include "ConfigChannel.h"
#include <iostream>

using namespace std;

ConfigChannel::ConfigChannel(ReadingClass& reading_class, int& dlc): m_valueType(reading_class, dlc) {
	reading_class.reading_uint16(m_multiplier);
	reading_class.reading_uint16(m_divider);
	reading_class.reading_uint16(m_offset);
			
	reading_class.reading_char(m_channel_name);
	reading_class.reading_char(m_unit_name);
	reading_class.reading_char(m_comment);
}

void ConfigChannel::print(){
	m_valueType.print();
	cout << "Multiplier: " << m_multiplier;
	cout << "\tDivder: " << m_divider;
	cout << "\tOffset: " << m_offset << endl;
			
	cout << "Channel name: " << m_channel_name;
	cout << "\tUnit name: " << m_unit_name  << endl;
	cout << "Comment: " << m_comment << endl << endl;
}

void ConfigChannel::writeToBinary(ofstream& data){
	m_valueType.writeToBinary(data);
	data.write(reinterpret_cast<char*>(&m_multiplier),sizeof(m_multiplier));
	data.write(reinterpret_cast<char*>(&m_divider),sizeof(m_divider));
	data.write(reinterpret_cast<char*>(&m_offset),sizeof(m_offset));
	data.write(reinterpret_cast<char*>(&m_channel_name),21);
	data.write(reinterpret_cast<char*>(&m_unit_name),21);
	data.write(reinterpret_cast<char*>(&m_comment),21);
}
