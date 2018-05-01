#pragma once
#include "ReadingClass.h"
#include "ConfigChannel.h"
#include <vector>
#include <fstream>
#include <stdint.h>

class ConfigFrame: public ReadingClass{
public:
	ConfigFrame(std::ifstream& data);
	void print();
	
protected:
	int m_iterator_DLC;

private:
	uint16_t m_ID;
	uint8_t m_DLC;
	char m_module_name[20];
	std::vector <ConfigChannel> m_channels;
};
