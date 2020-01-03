#include "ConfigFrame.h"
#include <cstdint>
#include <cmath>
#include "ConfigSignal.h"

using namespace std;

static const unsigned int MAX_ID_BITS = 11;
static const unsigned int MAX_ID_VALUE = (1<<MAX_ID_BITS)-1;
static const unsigned int MAX_DLC_VALUE = 8u;

static const unsigned int MODULE_NAME_LENGTH = 20;


ConfigFrame::ConfigFrame (unsigned int id, unsigned int dlc, string moduleName) : id(id), dlc(dlc), moduleName(moduleName) {
    if (this->id > MAX_ID_VALUE){
        throw std::invalid_argument("Value of id greather than max value.");
    }
    if (this->dlc > MAX_DLC_VALUE){
        throw std::invalid_argument("DLC exeeds maximum possible value.");
    }
}

ConfigFrame::ConfigFrame (ReadingClass& reader) : ConfigFrame(0, 0, "") {
    this->readFromBin(reader);
}

unsigned int ConfigFrame::getID() const {
    return this->id;
}

unsigned int ConfigFrame::getDLC() const {
    return this->dlc;
}

string ConfigFrame::getModuleName() const {
    return this->moduleName;
}

void ConfigFrame::setID(unsigned int id){
    if (id > MAX_ID_VALUE){
        throw std::invalid_argument("Value of id greather than max value");
    }
    this->id = id;
}

void ConfigFrame::setDLC(unsigned int dlc){
    if (dlc > MAX_DLC_VALUE){
        throw std::invalid_argument("DLC exeeds maximum possible value.");
    }
    for (ConfigSignal* pSignal: this->signalsVector){
        if ((pSignal->getStartBit() + pSignal->getLengthBits() + 7) / 8 > dlc){ //< calculating
            throw std::invalid_argument("One of signals assigned to the frame extends beyond new DLC of the frame.");
        }
    }
    this->dlc = dlc;
}

void ConfigFrame::setModuleName(string moduleName){
    this->moduleName = moduleName;
    this->moduleName.resize(MODULE_NAME_LENGTH);
}

ConfigSignal& ConfigFrame::getSignalByPosition(unsigned int position) {
    return *(this->signalsVector[position]);  //TODO sprawdzic czy poprawny argument
}

void ConfigFrame::addSignal(ConfigSignal* pSignal){ //TODO jakos odebrac dostep, zeby nie dalo sie tego wywolac. Jedynie w konstriktorze sygnalu

    if ((pSignal->getStartBit() + pSignal->getLengthBits() + 7) / 8 > this->getDLC()){
        throw std::invalid_argument("Signal exeeds maximum length of the frame.");
    }

    signalsVector.push_back(pSignal);
}

void ConfigFrame::removeAndDeleteSignalByPosition(unsigned int position){
    if (position < signalsVector.size()){
        ConfigSignal* pSignal = signalsVector.at(position);
        delete pSignal;
        signalsVector.erase(signalsVector.begin() + position);
    }
}

ConfigFrame::iterator ConfigFrame::begin() {
    return iterator(signalsVector.begin(), *this);
}

ConfigFrame::iterator ConfigFrame::end() {
    return iterator(signalsVector.end(), *this);
}

ConfigFrame::const_iterator ConfigFrame::cbegin() const {
    return const_iterator(signalsVector.begin(), *this);
}

ConfigFrame::const_iterator ConfigFrame::cend() const {
    return const_iterator(signalsVector.end(), *this);
}

void ConfigFrame::writeToBin(WritingClass& writer){

    writer.write_uint16(static_cast<unsigned int>(id), RawDataParser::UseDefaultEndian);
    writer.write_uint8(static_cast<unsigned int>(getDLC()));
    writer.write_string(moduleName, true, MODULE_NAME_LENGTH);
    for (vector <ConfigSignal*>::iterator it=signalsVector.begin(); it!=signalsVector.end(); it++){
        (*it)->writeToBin(writer);
    }
}

void ConfigFrame::readFromBin(ReadingClass& reader){

    setID(reader.reading_uint16(RawDataParser::UseDefaultEndian));
    unsigned int readDLC = min(MAX_DLC_VALUE, reader.reading_uint8());
    setModuleName(reader.reading_string(MODULE_NAME_LENGTH, true));

    unsigned int iteratorDLC = 0;

    this->setDLC(8); //temporary solution //TODO
    while(iteratorDLC < readDLC){
        ConfigSignal* pSignal = new ConfigSignal(this);
        pSignal->readFromBin(reader);
        pSignal->setStartBit(iteratorDLC / 8);

        iteratorDLC += (pSignal->getLengthBits() / 8);
    }

    this->setDLC(readDLC);
}

ConfigFrame::iterator::iterator(vector <ConfigSignal*>::iterator it, ConfigFrame& frameRef) :
innerIterator(it), frameReference(frameRef)
{
}

bool ConfigFrame::iterator::operator==(const ConfigFrame::iterator &second){
    return (innerIterator == second.innerIterator);
}

bool ConfigFrame::iterator::operator!=(const ConfigFrame::iterator &second){
    return (innerIterator != second.innerIterator);
}

ConfigSignal* ConfigFrame::iterator::operator*(){
    return *innerIterator;
}

ConfigSignal* ConfigFrame::iterator::operator->(){ //TODO do sprawdzenia
    return (*innerIterator);
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

ConfigFrame::const_iterator::const_iterator(vector <ConfigSignal*>::const_iterator it, const ConfigFrame& frameRef) :
innerIterator(it), frameReference(frameRef)
{
}

bool ConfigFrame::const_iterator::operator==(const ConfigFrame::const_iterator &second){
    return (innerIterator == second.innerIterator);
}

bool ConfigFrame::const_iterator::operator!=(const ConfigFrame::const_iterator &second){
    return (innerIterator != second.innerIterator);
}

const ConfigSignal* ConfigFrame::const_iterator::operator*() const {
    return *innerIterator;
}

const ConfigSignal* ConfigFrame::const_iterator::operator->() const { //TODO do sprawdzenia
    return (*innerIterator);
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

ConfigFrame::~ConfigFrame(){
    for (auto& pSignal : this->signalsVector){
        delete pSignal;
    }
}
