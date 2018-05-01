#pragma once
#include "ReadingClass.h"
#include "ValueType.h"
#include <stdint.h>
#include <fstream>


class ConfigChannel: public ReadingClass{
public:
	ConfigChannel(std::ifstream& data, int& dlc);
	void print();

private:
	ValueType m_valueType;
	uint16_t m_multiplier;
	uint16_t m_divider;
	uint16_t m_offset;
	char	m_channel_name[20];
	char	m_unit_name[20];
	char	m_comment[20];
};
