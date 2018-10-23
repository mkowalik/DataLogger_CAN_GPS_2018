#include "ConfigFrame.h"
#include <cstdint>

static const unsigned int MAX_ID_BITS = 11;
static const unsigned int MAX_ID_VALUE = (1<<MAX_ID_BITS)-1;
static const unsigned int MAX_DLC_VALUE = 8;

static const unsigned int MODULE_NAME_LENGTH = 20;

ConfigFrame::ConfigFrame(){

}

unsigned int ConfigFrame::get_ID(){
	return ID;
}

unsigned int ConfigFrame::get_DLC(){
	
	unsigned int dlc = 0;

	for (vector<ConfigChannel>::iterator it = channels.begin(); it != channels.end(); it++){
		dlc += it->get_DLC();
	}

	return dlc;

}

string ConfigFrame::get_moduleName(){
	return moduleName;
}

void ConfigFrame::set_ID(unsigned int aID){
	ID = min(aID, MAX_ID_VALUE);
}

void ConfigFrame::set_moduleName(string aModuleName){
    moduleName = aModuleName;
    moduleName.resize(MODULE_NAME_LENGTH);
}

vector <ConfigChannel> ConfigFrame::get_channels(){
	return channels;
}

vector <ConfigChannel>::iterator ConfigFrame::get_channels_begin_iterator(){
	return channels.begin();
}

vector <ConfigChannel>::iterator ConfigFrame::get_channels_end_iterator(){
	return channels.end();
}

void ConfigFrame::add_channel(ConfigChannel aChannel){
	channels.push_back(aChannel);
}

void ConfigFrame::write_bin(WritingClass& writer){

    writer.write_uint16(ID);
    writer.write_uint8(get_DLC());
    writer.write_string(moduleName, MODULE_NAME_LENGTH);
    for (vector <ConfigChannel>::iterator it=channels.begin(); it!=channels.end(); it++){
        it->write_bin(writer);
    }
}

void ConfigFrame::read_bin(ReadingClass& reader){

    set_ID(reader.reading_uint16());
    unsigned int readDLC = min(MAX_DLC_VALUE, reader.reading_uint8());
    set_moduleName(reader.reading_string(MODULE_NAME_LENGTH));

    unsigned int iteratorDLC = 0;

    while(iteratorDLC < readDLC){
        ConfigChannel channel;
        channel.read_bin(reader);
        add_channel(channel);

        iteratorDLC += channel.get_DLC();
    }
}

/*

void ConfigFrame::print(){
	std::cout << "ID of frame: " << m_ID;
	std::cout << "\tDLC: " << (int) m_DLC;
	std::cout << "\tModule name: " << m_module_name << std::endl << std::endl;

	for(uint32_t i=0; i<m_channels.size(); i++){
		m_channels[i].print();
	}
}

void ConfigFrame::writeToBinary(std::ofstream& data){
	data.write(reinterpret_cast<char*>(&m_ID),sizeof(m_ID));
	data.write(reinterpret_cast<char*>(&m_DLC),sizeof(m_DLC));
	data.write(reinterpret_cast<char*>(&m_module_name),21);
	for(uint32_t i=0; i<m_channels.size(); i++){
		m_channels[i].writeToBinary(data);
	}
}
*/
