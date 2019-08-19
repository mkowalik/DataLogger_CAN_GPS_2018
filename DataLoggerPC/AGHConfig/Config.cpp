#include "Config.h"
#include <iostream>
#include <algorithm>
#include <functional>

using namespace std;

int Config::get_actualVersion(){
    return Config::ACTUAL_VERSION;
}
int Config::get_actualSubVersion(){
    return Config::ACTUAL_SUB_VERSION;
}

Config::Config() : version(ACTUAL_VERSION), subVersion(ACTUAL_SUB_VERSION), canBitrate(DEFAULT_CAN_BITRATE)
{
}

int Config::get_version() const {
	return version;
}

int Config::get_subVersion() const {
	return subVersion;
}

Config::EnCANBitrate Config::get_CANBitrate() const {
    return canBitrate;
}

int Config::get_numOfFrames() const {
    return static_cast<int>(frames_map.size());
}

void Config::set_version(int sVersion){
    if (sVersion < 0 || sVersion > UINT16_MAX){
        throw std::invalid_argument("Version should be between 0 and UINT16_MAX");
    }
    version = sVersion;
}

void Config::set_subVersion(int sSubVersion){
    if (sSubVersion < 0 || sSubVersion > UINT16_MAX){
        throw std::invalid_argument("SubVersion should be between 0 and UINT16_MAX");
    }
    subVersion = sSubVersion;
}

void Config::set_CANBitrate(EnCANBitrate bitrate){
    this->canBitrate = bitrate;
}

ConfigFrame& Config::get_frame_by_id(int id) {
    if (frames_map.find(id) == frames_map.end()){
        throw std::out_of_range("Frame with given id does not exist");
    }
    return frames_map.at(id);
}

bool Config::has_frame_with_id(int id) const {
    return (frames_map.find(id) != frames_map.end());
}

void Config::add_frame(ConfigFrame& aFrame){
    frames_map.insert({aFrame.get_ID(), aFrame});
}

void Config::remove_frame_by_id(int id){
    if (frames_map.find(id) == frames_map.end()){
        throw std::out_of_range("Frame with given id does not exist");
    }
    frames_map.erase(id);
}

void Config::reset(){
    frames_map.clear();
}

Config::iterator Config::begin(){
    return iterator(frames_map.begin(), *this);
}

Config::iterator Config::end(){
    return iterator(frames_map.end(), *this);
}

Config::const_iterator Config::cbegin(){
    return const_iterator(frames_map.cbegin(), *this);
}

Config::const_iterator Config::cend(){
    return const_iterator(frames_map.cend(), *this);
}

void Config::write_to_bin(WritingClass& writer){

    writer.write_uint16(static_cast<unsigned int>(get_version()), RawDataParser::UseDefaultEndian);
    writer.write_uint16(static_cast<unsigned int>(get_subVersion()), RawDataParser::UseDefaultEndian);
    writer.write_uint16(static_cast<unsigned int>(get_CANBitrate()), RawDataParser::UseDefaultEndian);
    writer.write_uint16(static_cast<unsigned int>(get_numOfFrames()), RawDataParser::UseDefaultEndian);

    for (map<int, ConfigFrame>::iterator it=frames_map.begin(); it!=frames_map.end(); it++){
        it->second.write_to_bin(writer);
    }
}

void Config::read_from_bin(ReadingClass& reader){
    set_version(static_cast<int>(reader.reading_uint16(RawDataParser::UseDefaultEndian)));
    set_subVersion(static_cast<int>(reader.reading_uint16(RawDataParser::UseDefaultEndian)));

    if (this->get_version() != ACTUAL_VERSION || this->get_subVersion() != ACTUAL_SUB_VERSION){
        string exceptionString = "Version of read config file: ";
        exceptionString += to_string(this->get_version());
        exceptionString += ".";
        exceptionString += to_string(this->get_subVersion());
        exceptionString += " is different than used in the application: ";
        exceptionString += to_string(ACTUAL_VERSION);
        exceptionString += ".";
        exceptionString += to_string(ACTUAL_SUB_VERSION);
        exceptionString += ".";
        throw std::out_of_range(exceptionString);
    }

    set_CANBitrate(static_cast<EnCANBitrate>(reader.reading_uint16(RawDataParser::UseDefaultEndian)));
    int framesNumber = static_cast<int>(reader.reading_uint16(RawDataParser::UseDefaultEndian));

    for(int i=0; i<framesNumber; i++){
        ConfigFrame frame;
        frame.read_from_bin(reader);
        add_frame(frame);
    }
}

Config::iterator::iterator(map <int, ConfigFrame>::iterator it, Config& configRef) :
    innerIterator(it),
    configReference(configRef)
{
}

bool Config::iterator::operator==(const Config::iterator &second) const {
    return innerIterator == second.innerIterator;
}

bool Config::iterator::operator!=(const Config::iterator &second) const {
    return innerIterator != second.innerIterator;
}

ConfigFrame& Config::iterator::operator*(){
    return innerIterator->second;
}

ConfigFrame* Config::iterator::operator->(){
    return &(innerIterator->second);
}

Config::iterator& Config::iterator::operator++(){
    ++innerIterator;
    return *this;
}

Config::iterator Config::iterator::operator++(int){
    iterator ret(*this);
    ++innerIterator;
    return ret;
}

Config::const_iterator::const_iterator(map <int, ConfigFrame>::const_iterator it, const Config& configRef) :
    innerIterator(it),
    configReference(configRef)
{
}

bool Config::const_iterator::operator==(const Config::const_iterator &second) const {
    return innerIterator == second.innerIterator;
}

bool Config::const_iterator::operator!=(const Config::const_iterator &second) const {
    return innerIterator != second.innerIterator;
}

const ConfigFrame& Config::const_iterator::operator*(){
    return innerIterator->second;
}

const ConfigFrame* Config::const_iterator::operator->(){
    return &(innerIterator->second);
}

Config::const_iterator& Config::const_iterator::operator++(){
    ++innerIterator;
    return *this;
}

Config::const_iterator Config::const_iterator::operator++(int){
    const_iterator ret(*this);
    ++innerIterator;
    return ret;
}
