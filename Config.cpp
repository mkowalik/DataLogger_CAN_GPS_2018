#include "Config.h"
#include <iostream>

using namespace std;

Config::Config(std::string name_of_file): m_reading_object(name_of_file) {
	m_reading_object.reading_uint16(m_version);
	m_reading_object.reading_uint16(m_subversion);
	m_reading_object.reading_uint16(m_num_of_frames);
		
	for(int i=0; i<m_num_of_frames; ++i){
		ConfigFrame element(m_reading_object);
		m_frames.push_back(element);
	}
}

void Config::print(){
	std::cout << "Version: " << m_version;
	std::cout << "\tSubversion: " << m_subversion;
	std::cout << "\tNumber of frames: " << m_num_of_frames << std::endl << std::endl << std::endl;
	for (unsigned int i=0; i<m_frames.size(); i++){
		m_frames[i].print();
	}
}

void Config::writeToBinary(ofstream& data){
	data.write(reinterpret_cast<char*>(&m_version),sizeof(m_version));
	data.write(reinterpret_cast<char*>(&m_subversion),sizeof(m_subversion));
	data.write(reinterpret_cast<char*>(&m_num_of_frames),sizeof(m_num_of_frames));
	
	for(int i=0; i<m_num_of_frames; ++i){
		m_frames[i].writeToBinary(data);
	}

}
