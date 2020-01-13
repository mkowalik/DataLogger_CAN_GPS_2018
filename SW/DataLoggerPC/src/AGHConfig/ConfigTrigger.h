#ifndef CONFIGTRIGGER_H
#define CONFIGTRIGGER_H

#include "AGHUtils/ReadingClass.h"
#include "AGHUtils/WritingClass.h"

class Config;
class ConfigSignal;

class ConfigTrigger : public WritableToBin, public ReadableFromBin
{
public:
    enum class TrigerCompareOperator {
        EQUAL				= 0x01,
        NOT_EQUAL			= 0x02,
        GREATER             = 0x03,
        GREATER_OR_EQUAL	= 0x04,
        LESS				= 0x05,
        LESS_OR_EQUAL		= 0x06,
        BITWISE_AND         = 0x07,
        BITWISE_OR			= 0x08,
        BITWISE_XOR         = 0x09,
        FRAME_OCCURED		= 0x20,
        FRAME_TIMEOUT_MS	= 0x21
    };
private:
    const Config*           pConfig;
    const ConfigSignal*     pSignal;
    unsigned long long      compareConstValue;
    TrigerCompareOperator   compareOperator;
public:
    ConfigTrigger (ReadingClass& reader, const Config* pConfig);

    void                    setSignal(unsigned int frameID, unsigned int signalID);
    void                    setCompareConstValue(unsigned long long compareConstVal);
    void                    setCompareOperator(TrigerCompareOperator triggerCompareOperator);

    const ConfigSignal*     getSignal() const;
    unsigned long long      getCompareConstValue() const;
    TrigerCompareOperator   getCompareOperator() const;

public:
    bool operator==(const ConfigTrigger& b) const;

    //<----- ReadableFromBin, WritableToBin interfaces ----->//
    virtual void readFromBin(ReadingClass &reader) override;
    virtual void writeToBin(WritingClass &writer) override;

    ~ConfigTrigger(){

    }
};

#endif // CONFIGTRIGGER_H
