#include "ConfigTrigger.h"

#include "AGHConfig/Config.h"
#include "AGHConfig/ConfigSignal.h"

ConfigTrigger::ConfigTrigger (ReadingClass& reader, const Config* pConfig) : pConfig(pConfig)
{
    if (pConfig == nullptr){
        throw std::invalid_argument("Argument pConfig must not be nullptr.");
    }
    readFromBin(reader);
}

void ConfigTrigger::setSignal(unsigned int frameID, unsigned int signalID)
{
    pSignal = pConfig->getSignal(frameID, signalID);
}

void ConfigTrigger::setCompareConstValue(unsigned long long _compareConstVal)
{
    compareConstValue = _compareConstVal;
}

void ConfigTrigger::setCompareOperator(ConfigTrigger::TrigerCompareOperator _triggerCompareOperator)
{
    compareOperator = _triggerCompareOperator;
}

const ConfigSignal *ConfigTrigger::getSignal() const
{
    return pSignal;
}

unsigned long long ConfigTrigger::getCompareConstValue() const
{
    return compareConstValue;
}

ConfigTrigger::TrigerCompareOperator ConfigTrigger::getCompareOperator() const
{
    return compareOperator;
}

bool ConfigTrigger::operator==(const ConfigTrigger& b) const {
    if (getSignal() != b.getSignal()){
        return false;
    }
    if (getCompareConstValue() != b.getCompareConstValue()){
        return false;
    }
    if (getCompareOperator() != b.getCompareOperator()){
        return false;
    }
    return true;
}

void ConfigTrigger::readFromBin(ReadingClass &reader)
{
    unsigned int frameID = reader.reading_uint16(RawDataParser::UseDefaultEndian);
    unsigned int signalID = reader.reading_uint16(RawDataParser::UseDefaultEndian);

    pSignal = pConfig->getSignal(frameID, signalID);

    compareConstValue = reader.reading_uint64(RawDataParser::UseDefaultEndian);

    compareOperator = static_cast<TrigerCompareOperator>(reader.reading_uint8());
    switch (compareOperator) {
    case TrigerCompareOperator::EQUAL:
    case TrigerCompareOperator::NOT_EQUAL:
    case TrigerCompareOperator::GREATER:
    case TrigerCompareOperator::GREATER_OR_EQUAL:
    case TrigerCompareOperator::LESS:
    case TrigerCompareOperator::LESS_OR_EQUAL:
    case TrigerCompareOperator::BITWISE_AND:
    case TrigerCompareOperator::BITWISE_OR:
    case TrigerCompareOperator::BITWISE_XOR:
    case TrigerCompareOperator::FRAME_OCCURED:
    case TrigerCompareOperator::FRAME_TIMEOUT_MS:
        break;
    default:
        throw std::invalid_argument("Value of Trigger Compare Operator is invalid.");
    }
}

void ConfigTrigger::writeToBin(WritingClass &writer)
{
    writer.write_uint16(pSignal->getParentFrame()->getFrameID(), RawDataParser::UseDefaultEndian);
    writer.write_uint16(pSignal->getSignalID(), RawDataParser::UseDefaultEndian);
    writer.write_uint64(getCompareConstValue(), RawDataParser::UseDefaultEndian);
    writer.write_uint8(static_cast<unsigned int>(getCompareOperator()));
}
