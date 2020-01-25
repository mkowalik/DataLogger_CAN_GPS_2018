#include "ConfigSignal.h"

#include "AGHConfig/ConfigFrame.h"

#include <iostream>
#include <cstdint>

static constexpr unsigned int SIGNAL_NAME_LENGHT = 20;
static constexpr unsigned int UNIT_LENGTH = 20;
static constexpr unsigned int COMMENT_LENGTH = 20;

ConfigSignal::ConfigSignal(ConfigFrame* pParentFrame, ReadingClass& reader) :
    pParentFrame(pParentFrame),
    signalID(0),
    startBit(0),
    lengthBits(0),
    valueType(0),
    multiplier(1),
    divider(1),
    offset(0),
    signalName(""),
    unitName(""),
    comment("")
{
    readFromBin(reader);
}

ConfigSignal::ConfigSignal(ConfigFrame*   _parentFrame,
                           unsigned int   _signalID,
                           unsigned int   _startBit,
                           unsigned int   _lengthBits,
                           ValueType      _valueType,
                           int            _multiplier,
                           unsigned int   _divider,
                           int            _offset,
                           string         _signalName,
                           string         _unitName,
                           string         _comment
              )
    :
      pParentFrame(_parentFrame),
      signalID(_signalID),
      startBit(0),
      lengthBits(0),
      valueType(0),
      multiplier(1),
      divider(1),
      offset(0),
      signalName(""),
      unitName(""),
      comment("")
{
    if (_signalID > SIGNAL_ID_MAX_VALUE){
        std::invalid_argument("SignalID value may not exceed UINT16_MAX");
    }
    if (pParentFrame->hasSignalWithID(_signalID)){
        throw std::invalid_argument("Signal with given id already exists in the parent frame.");
    }
    setStartBit(_startBit);
    setLengthBits(_lengthBits);
    setValueType(_valueType);
    setMultiplier(_multiplier);
    setDivider(_divider);
    setOffset(_offset);
    setSignallName(_signalName);
    setUnitName(_unitName);
    setComment(_comment);
}

void ConfigSignal::_setSignalId(unsigned int _signalID)
{
    if (_signalID > SIGNAL_ID_MAX_VALUE){
        std::invalid_argument("SignalID value may not exceed UINT16_MAX");
    }
    if (pParentFrame->hasSignalWithID(_signalID)){
        throw std::invalid_argument("Signal with given id already exists in the parent frame.");
    }
    this->signalID = _signalID;
    pParentFrame->sortSignalsCallback();
}

ConfigFrame* ConfigSignal::getParentFrame() const {
    return this->pParentFrame;
}

unsigned int ConfigSignal::getSignalID() const {
    return signalID;
}

unsigned int ConfigSignal::getStartBit() const {
    return this->startBit;
}

unsigned int ConfigSignal::getLengthBits() const {
    return this->lengthBits;
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

void ConfigSignal::setSignalID(unsigned int _signalID)
{
    if (this->signalID != _signalID){
        _setSignalId(_signalID);
    }
}

void ConfigSignal::setStartBit(unsigned int startBit){
    if (startBit > SIGNAL_MAX_LENGTH_BITS){
        throw std::invalid_argument("Given start bit index exceed maximum length of the signal.");
    }
    if (this->getStartBit() + lengthBits > (ConfigFrame::MAX_DLC_VALUE * 8)){
        throw std::invalid_argument("Signal with given position and length exeeds max DLC of frame.");
    }
    this->startBit = startBit;
}

void ConfigSignal::setLengthBits(unsigned int lengthBits){
    if (lengthBits > ConfigSignal::SIGNAL_MAX_LENGTH_BITS){
        throw std::invalid_argument("Given length exceed maximum length of the signal.");
    }
    if (this->getStartBit() + lengthBits > (ConfigFrame::MAX_DLC_VALUE * 8)){
        throw std::invalid_argument("Signal with given position and length exeeds max DLC of frame.");
    }
    this->lengthBits = lengthBits;
}

void ConfigSignal::setValueType(ValueType aValueType){
    this->valueType = aValueType;
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

double ConfigSignal::convertRawValueToSymbolic(unsigned long long value) const {
    double ret = static_cast<double>(value);
    ret += static_cast<double>(getOffset());
    ret *= static_cast<double>(getMultiplier());
    ret /= static_cast<double>(getDivider());
    return ret;
}

long long ConfigSignal::convertRawValueToSymbolicInt(unsigned long long value) const {
    long long ret = static_cast<long long>(value);
    ret += static_cast<double>(getOffset());
    ret *= static_cast<double>(getMultiplier());
    ret /= static_cast<double>(getDivider());
    return ret;
}

unsigned long long ConfigSignal::convertSymbolicValueToRaw(double value) const
{
    value *= static_cast<double>(getDivider());
    value /= static_cast<double>(getMultiplier());
    value -= static_cast<double>(getOffset());
    if (value < 0.0){
        throw std::logic_error("Can't convert given symbolic value to raw value.");
    }
    return static_cast<unsigned long long>(value);
}

unsigned long long ConfigSignal::getRawValueFromFramePayload(std::vector<unsigned char> framePayload) const {

    if (getStartBit() + getLengthBits() > (framePayload.size() * 8)){
        throw std::invalid_argument("Signal with given position and length exeeds DLC of the frame.");
    }

    unsigned int bitsLeft       = getLengthBits();
    unsigned int bitsShiftRaw   = getStartBit() % 8;
    unsigned int bitIt          = getStartBit();
    unsigned long long ret      = 0;

    if (getValueType().isBigEndianType()){
        while (bitsLeft > 0) {
            unsigned char actualByte = framePayload.at(bitIt/8);
            unsigned char nextByte = 0;
            if ((bitIt/8)+1 < framePayload.size()) {
                nextByte = framePayload.at((bitIt/8)+1);
            }
            actualByte <<= bitsShiftRaw;
            actualByte |= (nextByte >> (8U - bitsShiftRaw));

            if (bitsLeft % 8 != 0){
                actualByte >>= (8U - (bitsLeft % 8));
                actualByte &= (0xFF >> (8U - (bitsLeft % 8)));
            }
            ret |= (static_cast<unsigned long long>(actualByte) << (bitsLeft - (((bitsLeft % 8U) == 0) ? 8U : (bitsLeft % 8U))) );
            bitIt += ((bitsLeft % 8U) == 0) ? 8U : (bitsLeft % 8U);
            bitsShiftRaw = (bitsShiftRaw + bitsLeft) % 8U;
            bitsLeft -= ((bitsLeft % 8U) == 0) ? 8U : (bitsLeft % 8U);
        }
    } else {
        while (bitsLeft > 0) {
            unsigned char actualByte = framePayload.at(bitIt/8);
            unsigned char nextByte = 0;
            if ((bitIt/8)+1 < framePayload.size()) {
                nextByte = framePayload.at((bitIt/8)+1);
            }
            actualByte <<= bitsShiftRaw;
            actualByte |= (nextByte >> (8U - bitsShiftRaw));

            if (bitsLeft < 8){
                actualByte >>= (8U - bitsLeft);
                actualByte &= (0xFF >> (8U - bitsLeft));
            }
            ret |= (static_cast<unsigned long long>(actualByte) << (((bitIt - getStartBit())/8) * 8U));
            bitIt += min(8U, bitsLeft);
            bitsLeft -= min(8U, bitsLeft);
        }
    }
    return ret;
}

double ConfigSignal::getSymbolicValueFromFramePayload(std::vector<unsigned char> framePayload) const {
    return convertRawValueToSymbolic(getRawValueFromFramePayload(framePayload));
}

long long ConfigSignal::getSymbolicIntValueFromFramePayload(std::vector<unsigned char> framePayload) const {
    return convertRawValueToSymbolicInt(getRawValueFromFramePayload(framePayload));
}

void ConfigSignal::writeToBin(WritingClass& writer){

    writer.write_uint16(getSignalID(), RawDataParser::UseDefaultEndian);
    writer.write_uint8(getStartBit());
    writer.write_uint8(getLengthBits());

    getValueType().writeToBin(writer);

    writer.write_int16(getMultiplier(), RawDataParser::UseDefaultEndian);
    writer.write_uint16(getDivider(), RawDataParser::UseDefaultEndian);
    writer.write_int16(getOffset(), RawDataParser::UseDefaultEndian);

    writer.write_string(getSignalName(), true, SIGNAL_NAME_LENGHT);
    writer.write_string(getUnitName(), true, UNIT_LENGTH);
    writer.write_string(getComment(), true, COMMENT_LENGTH);

    //TODO write named values
}


void ConfigSignal::readFromBin(ReadingClass& reader){

    _setSignalId(reader.reading_uint16(RawDataParser::UseDefaultEndian));
    setStartBit(reader.reading_uint8());
    setLengthBits(reader.reading_uint8());

    valueType.readFromBin(reader);

    setMultiplier(reader.reading_int16(RawDataParser::UseDefaultEndian));
    setDivider(reader.reading_uint16(RawDataParser::UseDefaultEndian));
    setOffset(reader.reading_int16(RawDataParser::UseDefaultEndian));

    setSignallName(reader.reading_string(SIGNAL_NAME_LENGHT, true));
    setUnitName(reader.reading_string(UNIT_LENGTH, true));
    setComment(reader.reading_string(COMMENT_LENGTH, true));

    //TODO read named values
}

ConfigSignal::~ConfigSignal(){
}
