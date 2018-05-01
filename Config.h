#pragma once
#include "ReadingClass.h"
#include "ConfigFrame.h"
#include <stdint.h>
#include <fstream>
#include <vector>

class Config: public ReadingClass{
public:
	Config(std::ifstream& data);
	void print();
	
private:
	uint16_t m_version;
	uint16_t m_subversion;
	uint16_t m_num_of_frames;
	std::vector <ConfigFrame> m_frames;

};
