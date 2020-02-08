#include "Config.h"

#include <iostream>
#include <algorithm>
#include <functional>
#include <algorithm>

//<--------------------------->//
//<----- Private methods ----->//
//<--------------------------->//

Config::Config()
    :
      version(ACTUAL_VERSION),
      subVersion(ACTUAL_SUB_VERSION),
      logFileName(),
      canBitrate(DEFAULT_CAN_BITRATE),
      gpsFrequency(DEFAULT_GPS_FREQUENCY),
      rtcConfigurationFrameID(DEFAULT_RTC_CONFIGURATION_FRAME_ID),
      framesVector(),
      startConfigTriggers(),
      stopConfigTriggers()
{
}

void Config::addFrame(ConfigFrame* pFrame){
    if (hasFrameWithId(pFrame->getFrameID())){
        throw std::invalid_argument("Frame with given ID already exists the config.");
    }
    if (pFrame->getFrameID() == getRTCConfigurationFrameID()){
        throw std::invalid_argument("Frame ID and RTC configuration Frame ID must not be equal.");
    }
    if (getFramesNumber() >= MAX_FRAMES_NUMBER){
        throw std::invalid_argument("Max number of frames is reached.");
    }
    framesVector.emplace(lowerBoundFrameConstIterator(pFrame->getFrameID()), pFrame);
}

void Config::addStartTrigger(ConfigTrigger *_pTrigger)
{
    if (getStartTriggersNumber() >= START_CONFIG_TRIGGERS_MAX_NUMBER){
        throw std::logic_error("Max number of start config triggers reached.");
    }
    for (const ConfigTrigger* pTrigger: startConfigTriggers){
        if ((*pTrigger)==(*_pTrigger)){
            throw std::invalid_argument("Given trigger is already defined.");
        }
    }
    startConfigTriggers.emplace_back(_pTrigger);
}

void Config::addStopTrigger(ConfigTrigger *_pTrigger)
{
    if (getStopTriggersNumber() >= STOP_CONFIG_TRIGGERS_MAX_NUMBER){
        throw std::logic_error("Max number of stop config triggers reached.");
    }
    for (auto pTrigger: stopConfigTriggers){
        if ((*pTrigger)==(*_pTrigger)){
            throw std::invalid_argument("Given trigger is already defined.");
        }
    }
    stopConfigTriggers.emplace_back(_pTrigger);
}

std::vector<ConfigFrame*>::const_iterator Config::lowerBoundFrameConstIterator(unsigned int frameID) const {
    return std::lower_bound(framesVector.cbegin(), framesVector.cend(), frameID, [](const ConfigFrame* pFrame, unsigned int frameID){
        return (pFrame->getFrameID() < frameID);
    });
}

std::vector<ConfigFrame*>::iterator Config::lowerBoundFrameIterator(unsigned int frameID) {
    return std::lower_bound(framesVector.begin(), framesVector.end(), frameID, [](const ConfigFrame* pFrame, unsigned int frameID){
        return (pFrame->getFrameID() < frameID);
    });
}

void Config::sortFramesCallback()
{
    std::sort(framesVector.begin(), framesVector.end(), [](ConfigFrame* pFr1, ConfigFrame* pFr2){return (pFr1->getFrameID() < pFr2->getFrameID());});
}

bool Config::isMaxSignalsNumber() const
{
    unsigned int signalsCounter = 0;
    for (auto pFrame : framesVector){
        signalsCounter += pFrame->getSignalsNumber();
    }
    return (signalsCounter == MAX_SIGNALS_NUMBER);
}

//<-------------------------->//
//<----- Public methods ----->//
//<-------------------------->//

Config::Config(std::string logFileName, EnCANBitrate canBitrate, EnGPSFrequency gpsFrequency, unsigned int rtcConfigurationFrameID)
    : Config() {
    setLogFileName(logFileName);
    setCANBitrate(canBitrate);
    setGPSFrequency(gpsFrequency);
    setRTCConfigurationFrameID(rtcConfigurationFrameID);
}

Config::Config(unsigned int version, unsigned int subVersion, std::string logFileName, EnCANBitrate canBitrate, EnGPSFrequency gpsFrequency, unsigned int rtcConfigurationFrameID)
    : Config() {
    setVersion(version);
    setSubVersion(subVersion);
    setLogFileName(logFileName);
    setCANBitrate(canBitrate);
    setGPSFrequency(gpsFrequency);
    setRTCConfigurationFrameID(rtcConfigurationFrameID);
}

Config::Config(ReadingClass& reader) : Config() {
    readFromBin(reader);
}

//<----- Access to preambule data ----->/

void Config::setVersion(unsigned int sVersion){
    if (sVersion > UINT16_MAX){
        throw std::invalid_argument("Version should be between 0 and UINT16_MAX");
    }
    version = sVersion;
}

void Config::setSubVersion(unsigned int sSubVersion){
    if (sSubVersion > UINT16_MAX){
        throw std::invalid_argument("SubVersion should be between 0 and UINT16_MAX");
    }
    subVersion = sSubVersion;
}

void Config::setLogFileName(std::string _logFileName)
{
    _logFileName.resize(std::min(_logFileName.length(), static_cast<unsigned long long>(CONFIG_NAME_LENGTH)));
    if (_logFileName.find_first_of(static_cast<char>(0)) != std::string::npos){
        _logFileName.resize(_logFileName.find_first_of(static_cast<char>(0)));
    }
    this->logFileName = _logFileName;
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

void Config::setRTCConfigurationFrameID(unsigned int frameID)
{
    if (frameID > ConfigFrame::MAX_ID_VALUE){
        throw std::invalid_argument("Value of ID greather than max value");
    }
    for (auto pFrame : framesVector){
        if (pFrame->getFrameID() == frameID){
            throw std::invalid_argument("RTC configuration Frame ID must not be equal to ID of any added frame.");
        }
    }
    this->rtcConfigurationFrameID = frameID;
}

unsigned int Config::getVersion() const {
	return version;
}

unsigned int Config::getSubVersion() const {
    return subVersion;
}

std::string Config::getLogFileName() const
{
    return logFileName;
}

Config::EnCANBitrate Config::getCANBitrate() const {
    return canBitrate;
}

Config::EnGPSFrequency Config::getGPSFrequency() const {
    return gpsFrequency;
}

unsigned int Config::getRTCConfigurationFrameID() const {
    return rtcConfigurationFrameID;
}

//<----- Access to frames definitions ----->/

unsigned int Config::getFramesNumber() const {
    return static_cast<unsigned int>(framesVector.size());
}

bool Config::framesEmpty() const {
    return framesVector.empty();
}

ConfigFrame* Config::addFrame(unsigned int frameID, unsigned int expectedDLC, std::string frameName)
{
    ConfigFrame* pFrame = new ConfigFrame(this, frameID, expectedDLC, frameName);
    try {
        addFrame(pFrame);
    } catch (const std::exception& e){
        delete pFrame;
        throw e;
    }
    return pFrame;
}

void Config::removeFrame(const FramesIterator frameIt){
    removeTriggersWithFrame(*frameIt);
    delete (*frameIt);
    framesVector.erase(frameIt);
}

void Config::removeFrame(unsigned int frameID){

    auto frameIt = lowerBoundFrameConstIterator(frameID);
    if ((frameIt == framesVector.cend()) || ((*frameIt)->getFrameID() != frameID)){
        throw std::out_of_range("Frame with given ID does not exist");
    }
    removeTriggersWithFrame(*frameIt);
    delete (*frameIt);
    framesVector.erase(frameIt);
}

bool Config::hasFrameWithId(unsigned int frameID) const {
    auto frameIt = lowerBoundFrameConstIterator(frameID);
    return ((frameIt != framesVector.cend()) && ((*frameIt)->getFrameID() == frameID));
}

ConfigFrame* Config::getFrameWithId(unsigned int frameID) const {
    auto frameIt = lowerBoundFrameConstIterator(frameID);
    if ((frameIt == framesVector.cend()) || ((*frameIt)->getFrameID() != frameID)){
        throw std::out_of_range("Frame with given ID does not exist.");
    }
    return (*frameIt);
}

bool Config::hasSignal(unsigned int frameID, unsigned int signalID) const {
    const ConfigFrame* pFrame = getFrameWithId(frameID);
    return pFrame->hasSignalWithID(signalID);
}

ConfigSignal * Config::getSignal(unsigned int frameID, unsigned int signalID) const {
    const ConfigFrame* pFrame = getFrameWithId(frameID);
    return pFrame->getSignalWithID(signalID);
}

Config::FramesIterator Config::beginFrames() {
    return framesVector.begin();
}

Config::FramesIterator Config::endFrames() {
    return framesVector.end();
}

Config::ConstFramesIterator Config::cbeginFrames() const {
    return framesVector.cbegin();
}

Config::ConstFramesIterator Config::cendFrames() const {
    return framesVector.cend();
}

//<----- Access to triggers definitions ----->/

unsigned int Config::getNumberOfStartTriggers()
{
    return static_cast<unsigned int>(startConfigTriggers.size());
}

unsigned int Config::getNumberOfStopTriggers()
{
    return static_cast<unsigned int>(stopConfigTriggers.size());
}

ConfigTrigger* Config::addStartTrigger(std::string triggerName, ConfigTrigger::FrameSignalVariant frameSignalVariant, unsigned long compareConstValue, ConfigTrigger::TriggerCompareOperator compareOperator){
    ConfigTrigger* pNewTrigger = new ConfigTrigger(this, triggerName, frameSignalVariant, compareConstValue, compareOperator);
    addStartTrigger(pNewTrigger);
    return pNewTrigger;
}

void Config::removeStartTrigger(Config::TriggersIterator trigIt)
{
    delete (*trigIt);
    startConfigTriggers.erase(trigIt);
}

void Config::removeStartTrigger(ConfigTrigger *pTriggerToRemove)
{
    for (TriggersIterator it = startConfigTriggers.begin(); it != startConfigTriggers.end() ; it++){
        if ((*it) == pTriggerToRemove){
            removeStartTrigger(it);
            return;
        }
    }
    throw std::logic_error("Given start triger wanted to be removed not found error.");
}

unsigned int Config::getStartTriggersNumber()
{
    return static_cast<unsigned int>(startConfigTriggers.size());
}

ConfigTrigger* Config::addStopTrigger(std::string triggerName, ConfigTrigger::FrameSignalVariant frameSignalVariant, unsigned long compareConstValue, ConfigTrigger::TriggerCompareOperator compareOperator){
    ConfigTrigger* pNewTrigger = new ConfigTrigger(this, triggerName, frameSignalVariant, compareConstValue, compareOperator);
    addStopTrigger(pNewTrigger);
    return pNewTrigger;
}

void Config::removeStopTrigger(Config::TriggersIterator trigIt)
{
    delete (*trigIt);
    stopConfigTriggers.erase(trigIt);
}

void Config::removeStopTrigger(ConfigTrigger *pTriggerToRemove)
{
    for (TriggersIterator it = stopConfigTriggers.begin(); it != stopConfigTriggers.end() ; it++){
        if ((*it) == pTriggerToRemove){
            removeStopTrigger(it);
            return;
        }
    }
    throw std::logic_error("Given stop triger wanted to be removed not found error.");
}

unsigned int Config::getStopTriggersNumber()
{
    return static_cast<unsigned int>(stopConfigTriggers.size());
}

Config::TriggersIterator Config::beginStartTriggers()
{
    return startConfigTriggers.begin();
}

Config::TriggersIterator Config::endStartTriggers()
{
    return startConfigTriggers.end();
}

Config::ConstTriggersIterator Config::cbeginStartTriggers() const
{
    return startConfigTriggers.cbegin();
}

Config::ConstTriggersIterator Config::cendStartTriggers() const
{
    return startConfigTriggers.cend();
}

Config::TriggersIterator Config::beginStopTriggers()
{
    return stopConfigTriggers.begin();
}

Config::TriggersIterator Config::endStopTriggers()
{
    return stopConfigTriggers.end();
}

Config::ConstTriggersIterator Config::cbeginStopTriggers() const
{
    return stopConfigTriggers.cbegin();
}

Config::ConstTriggersIterator Config::cendStopTriggers() const
{
    return stopConfigTriggers.cend();
}

void Config::removeTriggersWithSignal(const ConfigSignal *pSignal)
{
    for (auto it = startConfigTriggers.cbegin(); it != startConfigTriggers.cend();){
        if ((*it)->getSignal() == pSignal){
            delete (*it);
            it = startConfigTriggers.erase(it);
        } else {
            ++it;
        }
    }
    for (auto it = stopConfigTriggers.cbegin(); it != stopConfigTriggers.cend();){
        if ((*it)->getSignal() == pSignal){
            delete (*it);
            it = stopConfigTriggers.erase(it);
        } else {
            ++it;
        }
    }
}

void Config::removeTriggersWithFrame(const ConfigFrame *pFrame)
{
    for (auto it = startConfigTriggers.begin(); it != startConfigTriggers.end();){
        if ((*it)->getFrame() == pFrame){
            delete *it;
            it = startConfigTriggers.erase(it);
        } else {
            ++it;
        }
    }
    for (auto it = stopConfigTriggers.begin(); it != stopConfigTriggers.end();){
        if ((*it)->getFrame() == pFrame){
            delete *it;
            it = stopConfigTriggers.erase(it);
        } else {
            ++it;
        }
    }
}

void Config::reset(){
    for (auto pFrame : framesVector){
        delete pFrame;
    }
    framesVector.clear();
    for (auto pTrigger : startConfigTriggers){
        delete pTrigger;
    }
    startConfigTriggers.clear();
    for (auto pTrigger : stopConfigTriggers){
        delete pTrigger;
    }
    stopConfigTriggers.clear();
    logFileName.clear();
    canBitrate              = DEFAULT_CAN_BITRATE;
    gpsFrequency            = DEFAULT_GPS_FREQUENCY;
    rtcConfigurationFrameID = DEFAULT_RTC_CONFIGURATION_FRAME_ID;
}

void Config::writeToBin(WritingClass& writer){

    writer.write_uint16(static_cast<unsigned int>(getVersion()));
    writer.write_uint16(static_cast<unsigned int>(getSubVersion()));
    writer.write_string(getLogFileName(), true, CONFIG_NAME_LENGTH);
    writer.write_uint16(static_cast<unsigned int>(getCANBitrate()));
    writer.write_uint8(static_cast<unsigned int>(getGPSFrequency()));
    writer.write_uint16(getRTCConfigurationFrameID());

    writer.write_uint16(static_cast<unsigned int>(getFramesNumber()));
    for (auto& pFrame : framesVector){
        pFrame->writeToBin(writer);
    }

    writer.write_uint8(getNumberOfStartTriggers());
    writer.write_uint8(getNumberOfStopTriggers());

    for (auto& startTrigger : startConfigTriggers){
        startTrigger->writeToBin(writer);
    }

    for (auto& stopTrigger : stopConfigTriggers){
        stopTrigger->writeToBin(writer);
    }
}

void Config::readFromBin(ReadingClass& reader){

    this->reset();

    setVersion(reader.reading_uint16());
    setSubVersion(reader.reading_uint16());

    if (this->getVersion() != ACTUAL_VERSION || this->getSubVersion() != ACTUAL_SUB_VERSION){
        std::string exceptionString = "Version of read config file: ";
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

    setLogFileName(reader.reading_string(CONFIG_NAME_LENGTH, true));

    setCANBitrate(static_cast<EnCANBitrate>(reader.reading_uint16()));
    setGPSFrequency(static_cast<EnGPSFrequency>(reader.reading_uint8()));
    setRTCConfigurationFrameID(reader.reading_uint16());

    unsigned int framesNumber = reader.reading_uint16();
    for(unsigned int i=0; i<framesNumber; i++){
        ConfigFrame* pFrame = new ConfigFrame(this, reader);
        addFrame(pFrame);
    }

    unsigned int startLogTriggersNumber = reader.reading_uint8();
    unsigned int stopLogTriggersNumber = reader.reading_uint8();

    if (startLogTriggersNumber > START_CONFIG_TRIGGERS_MAX_NUMBER){
        throw std::out_of_range("START_LOG_TRIGGERS_NUMBER in config file is out of range.");
    }

    if (stopLogTriggersNumber > STOP_CONFIG_TRIGGERS_MAX_NUMBER){
        throw std::out_of_range("STOP_LOG_TRIGGERS_NUMBER in config file is out of range.");
    }

    for (unsigned int i=0; i<startLogTriggersNumber; i++){
        ConfigTrigger* pTrigger = new ConfigTrigger(this, reader);
        addStartTrigger(pTrigger);
    }

    for (unsigned int i=0; i<stopLogTriggersNumber; i++){
        ConfigTrigger* pTrigger = new ConfigTrigger(this, reader);
        addStopTrigger(pTrigger);
    }
}

Config::~Config(){
    this->reset();
}
