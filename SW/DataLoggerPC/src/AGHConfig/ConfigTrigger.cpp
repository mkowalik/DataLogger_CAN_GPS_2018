#include "ConfigTrigger.h"

#include "AGHConfig/Config.h"
#include "AGHConfig/ConfigSignal.h"

static constexpr unsigned int TRIGGER_NAME_LENGHT = 20;

const std::array<ConfigTrigger::TriggerCompareOperator, 11> ConfigTrigger::getAllCompareOperators()
{
    constexpr std::array<ConfigTrigger::TriggerCompareOperator, 11> values = {
        TriggerCompareOperator::EQUAL,
        TriggerCompareOperator::NOT_EQUAL,
        TriggerCompareOperator::GREATER,
        TriggerCompareOperator::GREATER_OR_EQUAL,
        TriggerCompareOperator::LESS,
        TriggerCompareOperator::LESS_OR_EQUAL,
        TriggerCompareOperator::BITWISE_AND,
        TriggerCompareOperator::BITWISE_OR,
        TriggerCompareOperator::BITWISE_XOR,
        TriggerCompareOperator::FRAME_OCCURED,
        TriggerCompareOperator::FRAME_TIMEOUT_MS
    };
    return values;
}

string ConfigTrigger::getTriggerCompareOperatorName(ConfigTrigger::TriggerCompareOperator oper)
{
    switch (oper){
        case TriggerCompareOperator::EQUAL:
            return "EQUAL";
        case TriggerCompareOperator::NOT_EQUAL:
            return "NOT_EQUAL";
        case TriggerCompareOperator::GREATER:
            return "GREATER";
        case TriggerCompareOperator::GREATER_OR_EQUAL:
            return "GREATER_OR_EQUAL";
        case TriggerCompareOperator::LESS:
            return "LESS";
        case TriggerCompareOperator::LESS_OR_EQUAL:
            return "LESS_OR_EQUAL";
        case TriggerCompareOperator::BITWISE_AND:
            return "BITWISE_AND";
        case TriggerCompareOperator::BITWISE_OR:
            return "BITWISE_OR";
        case TriggerCompareOperator::BITWISE_XOR:
            return "BITWISE_XOR";
        case TriggerCompareOperator::FRAME_OCCURED:
            return "FRAME_OCCURED";
        case TriggerCompareOperator::FRAME_TIMEOUT_MS:
            return "FRAME_TIMEOUT_MS";
    };
    throw std::invalid_argument("Wrong ConfigTrigger::TriggerCompareOperator value.");
}

string ConfigTrigger::getTriggerCompareOperatorSymbol(ConfigTrigger::TriggerCompareOperator oper)
{
    switch (oper){
        case TriggerCompareOperator::EQUAL:
            return "==";
        case TriggerCompareOperator::NOT_EQUAL:
            return "!=";
        case TriggerCompareOperator::GREATER:
            return ">";
        case TriggerCompareOperator::GREATER_OR_EQUAL:
            return ">=";
        case TriggerCompareOperator::LESS:
            return "<";
        case TriggerCompareOperator::LESS_OR_EQUAL:
            return "<=";
        case TriggerCompareOperator::BITWISE_AND:
            return "AND";
        case TriggerCompareOperator::BITWISE_OR:
            return "OR";
        case TriggerCompareOperator::BITWISE_XOR:
            return "XOR";
        case TriggerCompareOperator::FRAME_OCCURED:
            return "OCCURED";
        case TriggerCompareOperator::FRAME_TIMEOUT_MS:
            return "TIMEOUT";
    };
    throw std::invalid_argument("Wrong ConfigTrigger::TriggerCompareOperator value.");
}

ConfigTrigger::ConfigTrigger (const Config* pConfig, ReadingClass& reader) : pConfig(pConfig) {
    readFromBin(reader);
}

bool ConfigTrigger::isSignalUsedForOperator(ConfigTrigger::TriggerCompareOperator compareOperator)
{
    return ((compareOperator != ConfigTrigger::TriggerCompareOperator::FRAME_OCCURED) && (compareOperator != ConfigTrigger::TriggerCompareOperator::FRAME_TIMEOUT_MS));
}

bool ConfigTrigger::isConstCompareValueUsedForOperator(ConfigTrigger::TriggerCompareOperator compareOperator)
{
    return (compareOperator != ConfigTrigger::TriggerCompareOperator::FRAME_OCCURED);
}

void ConfigTrigger::setTriggerName(string _triggerName)
{
    if (_triggerName.length() > TRIGGER_NAME_LENGHT){
        _triggerName.resize(TRIGGER_NAME_LENGHT);
    }
    if (_triggerName.find_first_of(static_cast<char>(0)) != string::npos){
        _triggerName.resize(_triggerName.find_first_of(static_cast<char>(0)));
    }
    this->triggerName = _triggerName;

}

void ConfigTrigger::setFrameSignalOperator(const ConfigFrame * _pFrame, const ConfigSignal* _pSignal, ConfigTrigger::TriggerCompareOperator _oper)
{
    pFrame = _pFrame;
    compareOperator = _oper;
    if (!isSignalUsedForOperator(compareOperator)){
        pSignal = nullptr;
    } else {
        if (_pSignal == nullptr){
            throw std::logic_error("Signal may not be undefined. Value of signal necessery for operator.");
        }
        if (_pSignal->getParentFrame() != _pFrame){
            throw std::logic_error("Signal must be member of given frame.");
        }
        pSignal = _pSignal;
    }
}

void ConfigTrigger::setCompareConstValue(unsigned long long _value)
{
    compareConstValue = _value;
}

string ConfigTrigger::getTriggerName() const
{
    return triggerName;
}

ConfigTrigger::ConfigTrigger (const Config* _pConfig, std::string _triggerName, const ConfigFrame* _pFrame, const ConfigSignal* _pSignal, unsigned long long _compareConstValue, TriggerCompareOperator _compareOperator)
    :
      pConfig(_pConfig),
      compareConstValue(_compareConstValue)
{
    setFrameSignalOperator(_pFrame, _pSignal, _compareOperator);
    setTriggerName(_triggerName);
}

const ConfigFrame *ConfigTrigger::getFrame() const
{
    return pFrame;
}

const ConfigSignal *ConfigTrigger::getSignal() const
{
    return pSignal;
}

unsigned long long ConfigTrigger::getCompareConstValue() const
{
    return compareConstValue;
}

ConfigTrigger::TriggerCompareOperator ConfigTrigger::getCompareOperator() const
{
    return compareOperator;
}

bool ConfigTrigger::operator==(const ConfigTrigger& b) const {
    if (getFrame() != b.getFrame()){
        return false;
    }
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
    unsigned int frameID    = reader.reading_uint16(RawDataParser::UseDefaultEndian);
    unsigned int signalID   = reader.reading_uint16(RawDataParser::UseDefaultEndian);
    compareConstValue       = reader.reading_uint64(RawDataParser::UseDefaultEndian);
    compareOperator         = static_cast<TriggerCompareOperator>(reader.reading_uint8());

    switch (compareOperator) {
    case TriggerCompareOperator::EQUAL:
    case TriggerCompareOperator::NOT_EQUAL:
    case TriggerCompareOperator::GREATER:
    case TriggerCompareOperator::GREATER_OR_EQUAL:
    case TriggerCompareOperator::LESS:
    case TriggerCompareOperator::LESS_OR_EQUAL:
    case TriggerCompareOperator::BITWISE_AND:
    case TriggerCompareOperator::BITWISE_OR:
    case TriggerCompareOperator::BITWISE_XOR:
    case TriggerCompareOperator::FRAME_OCCURED:
    case TriggerCompareOperator::FRAME_TIMEOUT_MS:
        break;
    default:
        throw std::invalid_argument("Value of Trigger Compare Operator is invalid.");
    }

    pFrame = pConfig->getFrameWithId(frameID);
    if (isSignalUsedForOperator(compareOperator)){
        pSignal = pConfig->getSignal(frameID, signalID);
    } else {
        pSignal = nullptr;
    }
}

void ConfigTrigger::writeToBin(WritingClass &writer)
{
    writer.write_uint16(pSignal->getParentFrame()->getFrameID(), RawDataParser::UseDefaultEndian);
    writer.write_uint16(pSignal->getSignalID(), RawDataParser::UseDefaultEndian);
    writer.write_uint64(getCompareConstValue(), RawDataParser::UseDefaultEndian);
    writer.write_uint8(static_cast<unsigned int>(getCompareOperator()));
}
