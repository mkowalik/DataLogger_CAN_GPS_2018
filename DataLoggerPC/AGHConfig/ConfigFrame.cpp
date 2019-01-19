#include "ConfigFrame.h"
#include <cstdint>

static const unsigned int MAX_ID_BITS = 11;
static const unsigned int MAX_ID_VALUE = (1<<MAX_ID_BITS)-1;
static const unsigned int MAX_DLC_VALUE = 8;

static const unsigned int MODULE_NAME_LENGTH = 20;

unsigned int ConfigFrame::get_ID() const {
	return ID;
}

unsigned int ConfigFrame::get_DLC() const {
	
	unsigned int dlc = 0;

    for (vector<ConfigChannel>::const_iterator it = get_channels_begin_iterator(); it != get_channels_end_iterator(); it++){
		dlc += it->get_DLC();
	}

	return dlc;
}

string ConfigFrame::get_moduleName() const {
	return moduleName;
}

void ConfigFrame::set_ID(unsigned int aID){
	ID = min(aID, MAX_ID_VALUE);
}

void ConfigFrame::set_moduleName(string aModuleName){
    moduleName = aModuleName;
    moduleName.resize(MODULE_NAME_LENGTH);
}

vector <ConfigChannel>::const_iterator ConfigFrame::get_channels_begin_iterator() const {
    return channels.cbegin();
}

vector <ConfigChannel>::const_iterator ConfigFrame::get_channels_end_iterator() const {
    return channels.cend();
}

ConfigChannel& ConfigFrame::get_channel_by_position(int position) {
    return channels[position];  //TODO sprawdzic czy poprawne
}

void ConfigFrame::add_channel(ConfigChannel aChannel){
	channels.push_back(aChannel);
}

void ConfigFrame::remove_channel_by_position(int position){
    if (position < channels.size()){
        channels.erase(channels.begin() + position);
    }
}

void ConfigFrame::write_to_bin(WritingClass& writer){

    writer.write_uint16(ID);
    writer.write_uint8(get_DLC());
    writer.write_string(moduleName, MODULE_NAME_LENGTH);
    for (vector <ConfigChannel>::iterator it=channels.begin(); it!=channels.end(); it++){
        it->write_to_bin(writer);
    }
}

void ConfigFrame::read_from_bin(ReadingClass& reader){

    set_ID(reader.reading_uint16());
    unsigned int readDLC = min(MAX_DLC_VALUE, reader.reading_uint8());
    set_moduleName(reader.reading_string(MODULE_NAME_LENGTH));

    unsigned int iteratorDLC = 0;

    while(iteratorDLC < readDLC){
        ConfigChannel channel;
        channel.read_from_bin(reader);
        add_channel(channel);

        iteratorDLC += channel.get_DLC();
    }
}
