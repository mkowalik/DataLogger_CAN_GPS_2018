#include "ValueType.h"
#include "ReadingClass.h"

static const unsigned char CONFIG_SIGNED_TYPE_flag	(1<<0);
static const unsigned char CONFIG_16_BIT_TYPE_flag	(1<<1);
static const unsigned char CONFIG_ON_OFF_TYPE_flag	(1<<2);
static const unsigned char CONFIG_FLAG_TYPE_flag  	(1<<3);
static const unsigned char CONFIG_CUSTOM_TYPE_flag	(1<<4);


ValueType::ValueType(unsigned char aFeature){
	feature = aFeature;
}

unsigned int ValueType::channelDLC(){
	if (is16BitLength()){
		return 2;
	} 
	return 1;
}

bool ValueType::isSignedType(){
	return feature&CONFIG_SIGNED_TYPE_flag;
}

bool ValueType::is16BitLength(){
	return feature&CONFIG_16_BIT_TYPE_flag;
}

bool ValueType::isOnOffType(){
	return feature&CONFIG_ON_OFF_TYPE_flag;
}

bool ValueType::isFlagType(){
	return feature&CONFIG_FLAG_TYPE_flag;
}

bool ValueType::isCustomType(){
	return feature&CONFIG_CUSTOM_TYPE_flag;
}

void ValueType::write_bin(WritingClass& writer){

    unsigned int valueToWriteToFile = 0;

    if (isSignedType()){
        valueToWriteToFile |= CONFIG_SIGNED_TYPE_flag;
    }
    if (is16BitLength()){
        valueToWriteToFile |= CONFIG_16_BIT_TYPE_flag;
    }
    if (isOnOffType()){
        valueToWriteToFile |= CONFIG_ON_OFF_TYPE_flag;
    }
    if (isFlagType()){
        valueToWriteToFile |= CONFIG_FLAG_TYPE_flag;
    }
    if (isCustomType()){
        valueToWriteToFile |= CONFIG_CUSTOM_TYPE_flag;
    }

    writer.write_uint8(valueToWriteToFile);
}

void ValueType::read_bin(ReadingClass& reader){
    feature = reader.reading_uint8();
}

/*
bool ValueType::checkingBits(uint8_t feature, unsigned char bit){
	if ((feature & bit) != 0)
		return true;
	else
		return false;
}

void ValueType::print(){
	cout << "ValueType:\n";
	cout << "Signed: " << m_singed_type;
	cout << "\t16 bits: " << m_length_16_bit_type;
	cout << "\tOn: " << m_on_off_flag_type;
	cout << "\tFlag: " << m_flag_type;
	cout << "\tCustom type: " << m_custom_type << endl;
}



void ValueType::writeToBinary(ofstream& data){
	uint8_t number_to_write_to_file = 0;

	if (m_singed_type == true){
		number_to_write_to_file |= CONFIG_SIGNED_TYPE_flag;
	}
	if (m_length_16_bit_type == true){
		number_to_write_to_file |= CONFIG_16_BIT_TYPE_flag;
	}
	if (m_on_off_flag_type == true){
		number_to_write_to_file |= CONFIG_ON_OFF_TYPE_flag;
	}
	if (m_flag_type == true){
		number_to_write_to_file |= CONFIG_FLAG_TYPE_flag;
	}
	if (m_custom_type == true){
		number_to_write_to_file |= CONFIG_CUSTOM_TYPE_flag;
	}

	data.write(reinterpret_cast<char*>(&number_to_write_to_file),sizeof(number_to_write_to_file));
}
*/
