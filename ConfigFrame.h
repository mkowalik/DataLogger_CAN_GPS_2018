#ifndef CONFIGFRAME_H
#define CONFIGFRAME_H
#include "ReadingClass.h"
#include "ConfigChannel.h"
#include <vector>
#include <fstream>
#include <stdint.h>

class ConfigFrame{
public:
	ConfigFrame(ReadingClass& reading_object);
	void print();
	void writeToBinary(std::ofstream& data);
	
protected:
	int32_t m_iterator_DLC;

private:
	uint16_t 	m_ID;
	uint8_t 	m_DLC;
	char		m_module_name[20];
	std::vector <ConfigChannel> m_channels;
};

#endif // CONFIGFRAME_H
