#include "ConfigFrame.h"

#include <iostream>

using namespace std;

ConfigFrame::ConfigFrame(std::ifstream& data){
	reading_uint16(m_ID,data);
	reading_uint8(m_DLC,data);
	reading_char(m_module_name,data);
	m_iterator_DLC = 0;

	while(m_iterator_DLC != (int) m_DLC)
	{
		ConfigChannel element(data,m_iterator_DLC);
		m_channels.push_back(element);
	}
}

void ConfigFrame::print(){
	std::cout << "ID of frame: " << m_ID;
	std::cout << "\tDLC: " << (int) m_DLC;
	std::cout << "\tModule name: " << m_module_name << std::endl << std::endl;

	for(unsigned int i=0; i<m_channels.size(); i++)
		m_channels[i].print();
}
		