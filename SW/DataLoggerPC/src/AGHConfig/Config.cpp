#include "Config.h"
#include <iostream>
#include <algorithm>
#include <functional>

using namespace std;

int Config::getActualVersion(){
    return Config::ACTUAL_VERSION;
}
int Config::getActualSubVersion(){
    return Config::ACTUAL_SUB_VERSION;
}

Config::Config() : version(ACTUAL_VERSION), subVersion(ACTUAL_SUB_VERSION), canBitrate(DEFAULT_CAN_BITRATE), gpsFrequency(DEFAULT_GPS_FREQUENCY)
{
}

int Config::getVersion() const {
	return version;
}

int Config::getSubVersion() const {
	return subVersion;
}

Config::EnCANBitrate Config::getCANBitrate() const {
    return canBitrate;
}

Config::EnGPSFrequency Config::getGPSFrequency() const{
    return gpsFrequency;
}

int Config::getNumOfFrames() const {
    return static_cast<int>(idToFrameMap.size());
}

void Config::setVersion(int sVersion){
    if (sVersion < 0 || sVersion > UINT16_MAX){
        throw std::invalid_argument("Version should be between 0 and UINT16_MAX");
    }
    version = sVersion;
}

void Config::setSubVersion(int sSubVersion){
    if (sSubVersion < 0 || sSubVersion > UINT16_MAX){
        throw std::invalid_argument("SubVersion should be between 0 and UINT16_MAX");
    }
    subVersion = sSubVersion;
}

void Config::setCANBitrate(EnCANBitrate bitrate){
    if (bitrate == EnCANBitrate::bitrate_50kbps ||
            bitrate == EnCANBitrate::bitrate_125kbps ||
            bitrate == EnCANBitrate::bitrate_250kbps ||
            bitrate == EnCANBitrate::bitrate_500kbps ||
            bitrate == EnCANBitrate::bitrate_1Mbps){
        this->canBitrate = bitrate;
    } else {
        throw std::invalid_argument("Invalid value of CAN bitrate.");
    }
}

void Config::setGPSFrequency(EnGPSFrequency frequency){
    if (frequency == EnGPSFrequency::freq_GPS_OFF ||
            frequency == EnGPSFrequency::freq_0_5_Hz ||
            frequency == EnGPSFrequency::freq_1_Hz ||
            frequency == EnGPSFrequency::freq_2_Hz ||
            frequency == EnGPSFrequency::freq_5_Hz ||
            frequency == EnGPSFrequency::freq_10_Hz){
        this->gpsFrequency = frequency;
    } else {
        throw std::invalid_argument("Invalid value of GPS frequency.");
    }
}

ConfigFrame* Config::getFrameById(unsigned int id) {
    if (idToFrameMap.find(id) == idToFrameMap.end()){
        throw std::out_of_range("Frame with given id does not exist");
    }
    return idToFrameMap.at(id);
}

bool Config::hasFrameWithId(unsigned int id) const {
    return (idToFrameMap.find(id) != idToFrameMap.end());
}

void Config::addFrame(ConfigFrame* pFrame){
    idToFrameMap.insert({pFrame->getID(), pFrame});
}

void Config::removeFrameById(unsigned int id){
    if (idToFrameMap.find(id) == idToFrameMap.end()){
        throw std::out_of_range("Frame with given id does not exist");
    }
    delete idToFrameMap.at(id);
    idToFrameMap.erase(id);
}

void Config::reset(){
    for (auto& frame : idToFrameMap){
        delete frame.second;
    }
    idToFrameMap.clear();
}

Config::iterator Config::begin() {
    return iterator(idToFrameMap.begin(), this);
}

Config::iterator Config::end() {
    return iterator(idToFrameMap.end(), this);
}

Config::const_iterator Config::cbegin() const {
    return const_iterator(idToFrameMap.cbegin(), this);
}

Config::const_iterator Config::cend() const {
    return const_iterator(idToFrameMap.cend(), this);
}

void Config::writeToBin(WritingClass& writer){

    writer.write_uint16(static_cast<unsigned int>(getVersion()), RawDataParser::UseDefaultEndian);
    writer.write_uint16(static_cast<unsigned int>(getSubVersion()), RawDataParser::UseDefaultEndian);
    writer.write_uint16(static_cast<unsigned int>(getCANBitrate()), RawDataParser::UseDefaultEndian);
    writer.write_uint8(static_cast<unsigned int>(getGPSFrequency()));
    writer.write_uint16(static_cast<unsigned int>(getNumOfFrames()), RawDataParser::UseDefaultEndian);

    for (auto it=idToFrameMap.begin(); it!=idToFrameMap.end(); it++){
        it->second->writeToBin(writer);
    }
}

void Config::readFromBin(ReadingClass& reader){

    this->reset();

    setVersion(static_cast<int>(reader.reading_uint16(RawDataParser::UseDefaultEndian)));
    setSubVersion(static_cast<int>(reader.reading_uint16(RawDataParser::UseDefaultEndian)));

    if (this->getVersion() != ACTUAL_VERSION || this->getSubVersion() != ACTUAL_SUB_VERSION){
        string exceptionString = "Version of read config file: ";
        exceptionString += to_string(this->getVersion());
        exceptionString += ".";
        exceptionString += to_string(this->getSubVersion());
        exceptionString += " is different than used in the application: ";
        exceptionString += to_string(ACTUAL_VERSION);
        exceptionString += ".";
        exceptionString += to_string(ACTUAL_SUB_VERSION);
        exceptionString += ".";
        throw std::out_of_range(exceptionString);
    }

    setCANBitrate(static_cast<EnCANBitrate>(reader.reading_uint16(RawDataParser::UseDefaultEndian)));
    setGPSFrequency(static_cast<EnGPSFrequency>(reader.reading_uint8()));
    int framesNumber = static_cast<int>(reader.reading_uint16(RawDataParser::UseDefaultEndian));

    for(int i=0; i<framesNumber; i++){
        ConfigFrame* pFrame = new ConfigFrame(reader);
        addFrame(pFrame);
    }
}

Config::iterator::iterator(map <unsigned int, ConfigFrame*>::iterator it, Config* pConfig) :
    innerIterator(it),
    pConfig(pConfig)
{
}

bool Config::iterator::operator==(const Config::iterator &second) const {
    return innerIterator == second.innerIterator;
}

bool Config::iterator::operator!=(const Config::iterator &second) const {
    return innerIterator != second.innerIterator;
}

ConfigFrame* Config::iterator::operator*(){
    return innerIterator->second;
}

ConfigFrame* Config::iterator::operator->(){
    return innerIterator->second;
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

Config::const_iterator::const_iterator(map <unsigned int, ConfigFrame*>::const_iterator it, const Config* pConfig) :
    innerIterator(it),
    pConfig(pConfig)
{
}

bool Config::const_iterator::operator==(const Config::const_iterator &second) const {
    return innerIterator == second.innerIterator;
}

bool Config::const_iterator::operator!=(const Config::const_iterator &second) const {
    return innerIterator != second.innerIterator;
}

const ConfigFrame* Config::const_iterator::operator*(){
    return innerIterator->second;
}

const ConfigFrame* Config::const_iterator::operator->(){
    return innerIterator->second;
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

Config::~Config(){
    this->reset();
}
