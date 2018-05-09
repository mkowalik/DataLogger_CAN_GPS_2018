#include "ConfigFrame.h"
#include <iostream>

using namespace std;

ConfigFrame::ConfigFrame(ReadingClass& reading_object){
	reading_object.reading_uint16(m_ID);
	reading_object.reading_uint8(m_DLC);
	reading_object.reading_char(m_module_name);
	m_iterator_DLC = 0;

	while(m_iterator_DLC != (int) m_DLC){
		ConfigChannel element(reading_object, m_iterator_DLC);
		m_channels.push_back(element);
	}
}

void ConfigFrame::print(){
	std::cout << "ID of frame: " << m_ID;
	std::cout << "\tDLC: " << (int) m_DLC;
	std::cout << "\tModule name: " << m_module_name << std::endl << std::endl;

	for(uint32_t i=0; i<m_channels.size(); i++){
		m_channels[i].print();
	}
}

void ConfigFrame::writeToBinary(std::ofstream& data){
	data.write(reinterpret_cast<char*>(&m_ID),sizeof(m_ID));
	data.write(reinterpret_cast<char*>(&m_DLC),sizeof(m_DLC));
	data.write(reinterpret_cast<char*>(&m_module_name),21);
	for(uint32_t i=0; i<m_channels.size(); i++){
		m_channels[i].writeToBinary(data);
	}
}
