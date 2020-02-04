#include "ConfigFrame.h"

#include "AGHConfig/Config.h"

#include <cstdint>
#include <cmath>
#include <algorithm>

static constexpr unsigned int FRAME_NAME_LENGTH = 20;

//<--------------------------->//
//<----- Private methods ----->//
//<--------------------------->//

ConfigFrame::ConfigFrame (Config* _pConfig, unsigned int _frameId, unsigned int _expectedDLC, string _frameName) : pParentConfig(_pConfig), frameID(_frameId), expectedDLC(_expectedDLC), frameName(_frameName) {
    if (this->frameID > MAX_ID_VALUE){
        throw std::invalid_argument("Value of ID greather than max value.");
    }
    _setFrameID(_frameId);
    setExpectedDLC(_expectedDLC);
    setFrameName(_frameName);
}

ConfigFrame::ConfigFrame (Config* _pConfig, ReadingClass& _reader) : pParentConfig(_pConfig), frameID(0), frameName("") {
    this->readFromBin(_reader);
}

void ConfigFrame::addSignal(ConfigSignal* pSignal){

    if (pSignal->getStartBit() + pSignal->getLengthBits() > MAX_DLC_VALUE * 8){
        throw std::invalid_argument("Signal exeeds maximum length of the frame.");
    }
    if (hasSignalWithID(pSignal->getSignalID())){
        throw std::invalid_argument("Signal with given ID already exists in the frame.");
    }
    if (pParentConfig->isMaxSignalsNumber()){
        throw std::invalid_argument("Max number of signals had reached.");
    }
    signalsVector.insert(lowerBoundSignalConstIterator(pSignal->getSignalID()), pSignal);
}

std::vector<ConfigSignal*>::const_iterator ConfigFrame::lowerBoundSignalConstIterator(unsigned int signalID) const
{
    return std::lower_bound(signalsVector.cbegin(), signalsVector.cend(), signalID, [](const ConfigSignal* pSignal, unsigned int signalID){
        return (pSignal->getSignalID() < signalID);
    });
}

std::vector<ConfigSignal*>::iterator ConfigFrame::lowerBoundSignalIterator(unsigned int signalID)
{
    return std::lower_bound(signalsVector.begin(), signalsVector.end(), signalID, [](const ConfigSignal* pSignal, unsigned int signalID){
        return (pSignal->getSignalID() < signalID);
    });
}

void ConfigFrame::sortSignalsCallback()
{
    std::sort(signalsVector.begin(), signalsVector.end(), [](ConfigSignal* pSig1, ConfigSignal* pSig2){return (pSig1->getSignalID() < pSig2->getSignalID());});
}

//<-------------------------->//
//<----- Public methods ----->//
//<-------------------------->//

Config *ConfigFrame::getParentConfig() const
{
    return pParentConfig;
}

unsigned int ConfigFrame::getFrameID() const {
    return this->frameID;
}

unsigned int ConfigFrame::getExpextedDLC() const
{
    return expectedDLC;
}

string ConfigFrame::getFrameName() const {
    return this->frameName;
}

void ConfigFrame::_setFrameID(unsigned int _frameID){
    if (_frameID > MAX_ID_VALUE){
        throw std::invalid_argument("Value of ID greather than max value.");
    }
    if (pParentConfig->hasFrameWithId(_frameID)){
        throw std::invalid_argument("Frame with given ID already exists in the parent configuration.");
    }
    if (pParentConfig->getRTCConfigurationFrameID() == _frameID){
        throw std::invalid_argument("Frame ID and RTC configuration Frame ID must not be equal.");
    }
    frameID = _frameID;
    pParentConfig->sortFramesCallback();
}

void ConfigFrame::setExpectedDLC(unsigned int _expectedDLC)
{
    if (_expectedDLC > MAX_DLC_VALUE){
        throw std::logic_error("Expected DLC value exceeds maximum DLC length.");
    }
    for (auto pSignal : signalsVector){
        if ((pSignal->getStartBit() + pSignal->getLengthBits()) > (_expectedDLC * 8)){
            throw std::logic_error("Some of signals positions defined in given frame exeeds new Expected DLC value.");
        }
    }
    expectedDLC = _expectedDLC;
}

void ConfigFrame::setFrameName(string _frameName){
    _frameName.resize(std::min(_frameName.length(), static_cast<unsigned long long>(FRAME_NAME_LENGTH)));
    if (_frameName.find_first_of(static_cast<char>(0)) != string::npos){
        _frameName.resize(_frameName.find_first_of(static_cast<char>(0)));
    }
    this->frameName = _frameName;
}

void ConfigFrame::setFrameID(unsigned int _frameID){
    if (frameID != _frameID){
        _setFrameID(_frameID);
    }
}


ConfigSignal *ConfigFrame::addSignal(unsigned int signalID, unsigned int startBit, unsigned int lengthBits, ValueType valueType, int multiplier, unsigned int divider, int offset, string signalName, string unitName, string comment)
{
    ConfigSignal* pSignal = new ConfigSignal(this,
                                             signalID,
                                             startBit,
                                             lengthBits,
                                             valueType,
                                             multiplier,
                                             divider,
                                             offset,
                                             signalName,
                                             unitName,
                                             comment);
    try {
        addSignal(pSignal);
    } catch (const std::exception& e){
        delete pSignal;
        throw e;
    }
    return pSignal;
}

ConfigSignal *ConfigFrame::addSignal(unsigned int startBit, unsigned int lengthBits, ValueType valueType, int multiplier, unsigned int divider, int offset, string signalName, string unitName, string comment)
{
    ConfigSignal* pSignal = new ConfigSignal(this,
                                             signalsEmpty() ? 0 : signalsVector.back()->getSignalID() + 1,
                                             startBit,
                                             lengthBits,
                                             valueType,
                                             multiplier,
                                             divider,
                                             offset,
                                             signalName,
                                             unitName,
                                             comment);
    try {
        addSignal(pSignal);
    } catch (const std::exception& e){
        delete pSignal;
        throw e;
    }
    return pSignal;
}

void ConfigFrame::removeSignal(const ConfigFrame::SignalsIterator signalIt)
{
    pParentConfig->removeTriggersWithSignal(*signalIt);
    delete (*signalIt);
    signalsVector.erase(signalIt);
}

void ConfigFrame::removeSignal(unsigned int signalID)
{
    auto signalIt = lowerBoundSignalConstIterator(signalID);
    if ((signalIt == signalsVector.cend()) || ((*signalIt)->getSignalID() != signalID)){
        throw std::out_of_range("Signal with given ID does not exist");
    }

    pParentConfig->removeTriggersWithSignal(*signalIt);

    delete (*signalIt);
    signalsVector.erase(signalIt);
}

bool ConfigFrame::signalsEmpty() const
{
    return signalsVector.empty();
}

unsigned int ConfigFrame::getSignalsNumber() const
{
    return static_cast<unsigned int>(signalsVector.size());
}

ConfigFrame::SignalsIterator ConfigFrame::beginSignals() {
    return signalsVector.begin();
}

ConfigFrame::SignalsIterator ConfigFrame::endSignals() {
    return signalsVector.end();
}

ConfigFrame::ConstSignalsIterator ConfigFrame::cbeginSignals() const {
    return signalsVector.cbegin();
}

ConfigFrame::ConstSignalsIterator ConfigFrame::cendSignals() const {
    return signalsVector.cend();
}

bool ConfigFrame::hasSignalWithID(unsigned int signalID) const
{
    auto signalIt = lowerBoundSignalConstIterator(signalID);
    return ((signalIt != signalsVector.cend()) && ((*(signalIt))->getSignalID() == signalID));
}

ConfigSignal *ConfigFrame::getSignalWithID(unsigned int signalID) const
{
    auto signalIt = lowerBoundSignalConstIterator(signalID);
    if ((signalIt == signalsVector.cend()) || ((*(signalIt))->getSignalID() != signalID)){
        throw std::invalid_argument("Signal with given ID does not exist in this frame.");
    }
    return (*signalIt);
}

void ConfigFrame::writeToBin(WritingClass& writer){

    writer.write_uint16(getFrameID());
    writer.write_uint8(getExpextedDLC());
    writer.write_string(getFrameName(), true, FRAME_NAME_LENGTH);
    writer.write_uint16(getSignalsNumber());
    for (auto pSignal : signalsVector){
        pSignal->writeToBin(writer);
    }
}

void ConfigFrame::readFromBin(ReadingClass& reader){

    _setFrameID(reader.reading_uint16());
    setExpectedDLC(reader.reading_uint8());
    setFrameName(reader.reading_string(FRAME_NAME_LENGTH, true));

    unsigned int signalNumber = reader.reading_uint16();

    for (unsigned int i=0; i<signalNumber; i++){
        ConfigSignal* pSignal = new ConfigSignal(this, reader);
        addSignal(pSignal);
    }
}

ConfigFrame::~ConfigFrame(){
    for (auto pSignal : signalsVector){
        delete pSignal;
    }
    signalsVector.clear();
}
