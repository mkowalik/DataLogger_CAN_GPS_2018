#ifndef CONFIGCHANNEL_H
#define CONFIGCHANNEL_H
#include "ReadingClass.h"
#include "ValueType.h"
#include <stdint.h>
#include <fstream>
#define SIZE 20

class ConfigChannel{
public:
	ConfigChannel(ReadingClass& reading_class, int& dlc);
	void print();
	void writeToBinary(std::ofstream& data);

private:
	ValueType 	m_valueType;
	uint16_t 	m_multiplier;
	uint16_t 	m_divider;
	uint16_t 	m_offset;
	char		m_channel_name[SIZE];
	char		m_unit_name[SIZE];
	char		m_comment[SIZE];
};

#endif // CONFIGCHANNEL_H
