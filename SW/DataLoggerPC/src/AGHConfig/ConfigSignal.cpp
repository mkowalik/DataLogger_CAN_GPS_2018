#include "ConfigSignal.h"
#include <iostream>
#include <cstdint>

static const unsigned int SIGNAL_NAME_LENGHT = 20;
static const unsigned int UNIT_LENGTH = 20;
static const unsigned int COMMENT_LENGTH = 20;

ConfigSignal::ConfigSignal(ConfigFrame* parentFrame) :
    parentFrame(parentFrame),
    valueType(0),
    startBit(0),
    lengthBits(0),
    multiplier(1),
    divider(1),
    offset(0),
    signalName(""),
    unitName(""),
    comment("")
{
    parentFrame->addSignal(this);
}

const ConfigFrame* ConfigSignal::getParentFrame() const {
    return this->parentFrame;
}

unsigned int ConfigSignal::getLengthBits() const {
    return this->lengthBits;
}

unsigned int ConfigSignal::getStartBit() const {
    return this->startBit;
}

ValueType ConfigSignal::getValueType() const {
    return this->valueType;
}

int ConfigSignal::getMultiplier() const {
    return this->multiplier;
}

unsigned int ConfigSignal::getDivider() const {
    return this->divider;
}

int ConfigSignal::getOffset() const {
    return this->offset;
}

string ConfigSignal::getSignalName() const {
    return this->signalName;
}

string ConfigSignal::getUnitName() const {
    return this->unitName;
}

string ConfigSignal::getComment() const {
    return this->comment;
}

void ConfigSignal::setValueType(ValueType aValueType){
    this->valueType = aValueType;
}

void ConfigSignal::setLengthBits(unsigned int lengthBits){
    if ((this->getStartBit() + lengthBits + 7)/8 > this->parentFrame->getDLC()){
        throw std::invalid_argument("Signal with given length extends beyond DLC of the frame assigned to.");
    }
    if (lengthBits > ConfigSignal::SIGNAL_MAX_LENGTH_BITS){
        throw std::invalid_argument("Given length exceed maximum length of the signal.");
    }
    this->lengthBits = lengthBits;
}

void ConfigSignal::setStartBit(unsigned int startBit){
    this->startBit = startBit;
}

void ConfigSignal::setMultiplier(int aMultiplier){
    if ((aMultiplier < INT16_MIN) || (aMultiplier > INT16_MAX)){
        std::invalid_argument("Multiplier should be between INT16_MIN and INT16_MAX");
    }
    this->multiplier = aMultiplier;
}

void ConfigSignal::setDivider(unsigned int aDivider){
    if ((aDivider < 1) || (aDivider > UINT16_MAX)){
        std::invalid_argument("Divider should be between 1 and UINT16_MAX");
    }
    this->divider = aDivider;
}

void ConfigSignal::setOffset(int aOffset){
    if ((aOffset < INT16_MIN) || (aOffset > INT16_MAX)){
        std::invalid_argument("Offset should be between INT16_MIN and INT16_MAX");
    }
    this->offset = aOffset;
}

void ConfigSignal::setSignallName(string aSignalName){
    if (aSignalName.length() > SIGNAL_NAME_LENGHT){
        aSignalName.resize(SIGNAL_NAME_LENGHT);
    }
    if (aSignalName.find_first_of(static_cast<char>(0)) != string::npos){
        aSignalName.resize(aSignalName.find_first_of(static_cast<char>(0)));
    }
    this->signalName = aSignalName;
}

void ConfigSignal::setUnitName(string aUnitName){
    if (aUnitName.length() > UNIT_LENGTH){
        aUnitName.resize(UNIT_LENGTH);
    }
    if (aUnitName.find_first_of(static_cast<char>(0)) != string::npos){
        aUnitName.resize(aUnitName.find_first_of(static_cast<char>(0)));
    }
    this->unitName = aUnitName;
}

void ConfigSignal::setComment(string aComment){
    if (aComment.length() > COMMENT_LENGTH){
        aComment.resize(COMMENT_LENGTH);
    }
    if (aComment.find_first_of(static_cast<char>(0)) != string::npos){
        aComment.resize(aComment.find_first_of(static_cast<char>(0)));
    }
    this->comment = aComment;
}

bool ConfigSignal::hasNameForValue(int _channelValue){
    return (channelNamedValues.find(_channelValue) != channelNamedValues.end());
}

ConfigSignalNamedValue& ConfigSignal::getNamedValue(int _channelValue){
    if (channelNamedValues.find(_channelValue) == channelNamedValues.end()){
        throw std::out_of_range("Name for given value does not exist");
    }
    return channelNamedValues.at(_channelValue);
}

void ConfigSignal::addNamedValue(int _channelValue, ConfigSignalNamedValue _namedValue){
    channelNamedValues.insert(make_pair(_channelValue, _namedValue));
}

void ConfigSignal::writeToBin(WritingClass& writer){

    getValueType().writeToBin(writer);

    writer.write_int16(getMultiplier(), RawDataParser::UseDefaultEndian);
    writer.write_uint16(getDivider(), RawDataParser::UseDefaultEndian);
    writer.write_int16(getOffset(), RawDataParser::UseDefaultEndian);

    writer.write_string(getSignalName(), true, SIGNAL_NAME_LENGHT);
    writer.write_string(getUnitName(), true, UNIT_LENGTH);
    writer.write_string(getComment(), true, COMMENT_LENGTH);
}


void ConfigSignal::readFromBin(ReadingClass& reader){

    valueType.readFromBin(reader);

    setMultiplier(reader.reading_int16(RawDataParser::UseDefaultEndian));
    setDivider(reader.reading_uint16(RawDataParser::UseDefaultEndian));
    setOffset(reader.reading_int16(RawDataParser::UseDefaultEndian));

    setSignallName(reader.reading_string(SIGNAL_NAME_LENGHT, true));
    setUnitName(reader.reading_string(UNIT_LENGTH, true));
    setComment(reader.reading_string(COMMENT_LENGTH, true));

    this->setLengthBits(this->valueType.is16BitLength() ? 16 : 8); //TODO
}

ConfigSignal::~ConfigSignal(){
}
