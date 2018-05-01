#include "Config.h"

#include <iostream>

using namespace std;

Config::Config(std::ifstream& data){
	reading_uint16(m_version,data);
	reading_uint16(m_subversion,data);
	reading_uint16(m_num_of_frames,data);
		
	for(int i=0; i<m_num_of_frames; ++i)
	{
		ConfigFrame element(data);
		m_frames.push_back(element);
	}
}

void Config::print(){
	std::cout << "Version: " << m_version;
	std::cout << "\tSubversion: " << m_subversion;
	std::cout << "\tNumber of frames: " << m_num_of_frames << std::endl << std::endl << std::endl;
	for (unsigned int i=0; i<m_frames.size(); i++)
		m_frames[i].print();
	
}