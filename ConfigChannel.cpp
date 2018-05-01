#include "ConfigChannel.h"

#include <iostream>

using namespace std;

ConfigChannel::ConfigChannel(ifstream& data, int& dlc): m_valueType(data,dlc) {
	
	reading_uint16(m_multiplier, data);
	reading_uint16(m_divider,data);
	reading_uint16(m_offset,data);
			
	reading_char(m_channel_name,data);
	reading_char(m_unit_name,data);
	reading_char(m_comment,data);
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
