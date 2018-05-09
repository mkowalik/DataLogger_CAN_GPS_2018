#ifndef CONFIG_H
#define CONFIG_H
#include "ReadingClass.h"
#include "ConfigFrame.h"
#include <stdint.h>
#include <fstream>
#include <vector>

class Config{
public:
	Config(std::string name_of_file);
	void print();
	void writeToBinary(std::ofstream& data);
	
private:
	ReadingClass m_reading_object;
	uint16_t m_version;
	uint16_t m_subversion;
	uint16_t m_num_of_frames;
	std::vector <ConfigFrame> m_frames;
};

#endif // CONFIG_H
