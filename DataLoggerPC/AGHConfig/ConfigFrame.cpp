#include "ConfigFrame.h"
#include <cstdint>

using namespace std;

static const int MAX_ID_BITS = 11;
static const int MAX_ID_VALUE = (1<<MAX_ID_BITS)-1;
static const int MAX_DLC_VALUE = 8;

static const unsigned int MODULE_NAME_LENGTH = 20;

int ConfigFrame::get_ID() const {
	return ID;
}

int ConfigFrame::get_DLC() const {
	
    int dlc = 0;

    for (vector<ConfigChannel>::const_iterator it = channels.begin(); it != channels.end(); it++){
		dlc += it->get_DLC();
	}

	return dlc;
}

string ConfigFrame::get_moduleName() const {
	return moduleName;
}

void ConfigFrame::set_ID(int aID){
	ID = min(aID, MAX_ID_VALUE);
    ID = max(aID, 0);
}

void ConfigFrame::set_moduleName(string aModuleName){
    moduleName = aModuleName;
    moduleName.resize(MODULE_NAME_LENGTH);
}

ConfigChannel& ConfigFrame::get_channel_by_position(int position) {
    return channels[position];  //TODO sprawdzic czy poprawny argument
}

void ConfigFrame::add_channel(ConfigChannel aChannel){
	channels.push_back(aChannel);
}

void ConfigFrame::remove_channel_by_position(int position){
    if (position < static_cast<int>(channels.size())){
        channels.erase(channels.begin() + position);
    }
}

ConfigFrame::iterator ConfigFrame::begin() {
    return iterator(channels.begin(), *this);
}

ConfigFrame::iterator ConfigFrame::end() {
    return iterator(channels.end(), *this);
}

ConfigFrame::const_iterator ConfigFrame::cbegin() const {
    return const_iterator(channels.begin(), *this);
}

ConfigFrame::const_iterator ConfigFrame::cend() const {
    return const_iterator(channels.end(), *this);
}

void ConfigFrame::write_to_bin(WritingClass& writer){

    writer.write_uint16(static_cast<unsigned int>(ID), RawDataParser::UseDefaultEndian);
    writer.write_uint8(static_cast<unsigned int>(get_DLC()));
    writer.write_string(moduleName, true, MODULE_NAME_LENGTH);
    for (vector <ConfigChannel>::iterator it=channels.begin(); it!=channels.end(); it++){
        it->write_to_bin(writer);
    }
}

void ConfigFrame::read_from_bin(ReadingClass& reader){

    set_ID(static_cast<int>(reader.reading_uint16(RawDataParser::UseDefaultEndian)));
    int readDLC = min(MAX_DLC_VALUE, static_cast<int>(reader.reading_uint8()));
    set_moduleName(reader.reading_string(MODULE_NAME_LENGTH, true));

    int iteratorDLC = 0;

    while(iteratorDLC < readDLC){
        ConfigChannel channel;
        channel.read_from_bin(reader);
        add_channel(channel);

        iteratorDLC += channel.get_DLC();
    }
}

ConfigFrame::iterator::iterator(vector <ConfigChannel>::iterator it, ConfigFrame& frameRef) :
innerIterator(it), frameReference(frameRef)
{
}

bool ConfigFrame::iterator::operator==(const ConfigFrame::iterator &second){
    return (innerIterator == second.innerIterator);
}

bool ConfigFrame::iterator::operator!=(const ConfigFrame::iterator &second){
    return (innerIterator != second.innerIterator);
}

ConfigChannel& ConfigFrame::iterator::operator*(){
    return (*innerIterator);
}

ConfigChannel* ConfigFrame::iterator::operator->(){ //TODO do sprawdzenia
    return &(*innerIterator);
}

ConfigFrame::iterator& ConfigFrame::iterator::operator++(){
    ++innerIterator;
    return (*this);
}

ConfigFrame::iterator ConfigFrame::iterator::operator++(int){
    ConfigFrame::iterator ret(*this);
    ++innerIterator;
    return ret;
}


ConfigFrame::iterator& ConfigFrame::iterator::operator--(){
    --innerIterator;
    return (*this);
}

ConfigFrame::iterator ConfigFrame::iterator::operator--(int){
    ConfigFrame::iterator ret(*this);
    --innerIterator;
    return ret;
}

ConfigFrame::const_iterator::const_iterator(vector <ConfigChannel>::const_iterator it, const ConfigFrame& frameRef) :
innerIterator(it), frameReference(frameRef)
{
}

bool ConfigFrame::const_iterator::operator==(const ConfigFrame::const_iterator &second){
    return (innerIterator == second.innerIterator);
}

bool ConfigFrame::const_iterator::operator!=(const ConfigFrame::const_iterator &second){
    return (innerIterator != second.innerIterator);
}

const ConfigChannel& ConfigFrame::const_iterator::operator*() const {
    return (*innerIterator);
}

const ConfigChannel* ConfigFrame::const_iterator::operator->() const { //TODO do sprawdzenia
    return &(*innerIterator);
}

ConfigFrame::const_iterator& ConfigFrame::const_iterator::operator++(){
    ++innerIterator;
    return (*this);
}

ConfigFrame::const_iterator ConfigFrame::const_iterator::operator++(int){
    ConfigFrame::const_iterator ret(*this);
    ++innerIterator;
    return ret;
}

ConfigFrame::const_iterator& ConfigFrame::const_iterator::operator--(){
    --innerIterator;
    return (*this);
}

ConfigFrame::const_iterator ConfigFrame::const_iterator::operator--(int){
    ConfigFrame::const_iterator ret(*this);
    --innerIterator;
    return ret;
}
