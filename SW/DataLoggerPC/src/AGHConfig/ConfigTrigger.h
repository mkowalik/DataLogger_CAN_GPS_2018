#ifndef CONFIGTRIGGER_H
#define CONFIGTRIGGER_H

#include <array>
#include <variant>

#include "AGHUtils/ReadingClass.h"
#include "AGHUtils/WritingClass.h"

class Config;
class ConfigFrame;
class ConfigSignal;

class ConfigTrigger : public WritableToBin, public ReadableFromBin {
    friend class Config;
public:
    enum class TriggerCompareOperator {
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

    using FrameSignalVariant = std::variant<const ConfigFrame*, const ConfigSignal*>;

    static const std::array<ConfigTrigger::TriggerCompareOperator, 11> getAllCompareOperators();
    static std::string                         getTriggerCompareOperatorName(TriggerCompareOperator oper);
    static std::string                         getTriggerCompareOperatorSymbol(TriggerCompareOperator oper);

private:
    std::string                 triggerName;
    const Config*               pConfig;
    FrameSignalVariant          vpFrameSignal;
    unsigned long               compareConstValue;
    TriggerCompareOperator      compareOperator;

    ConfigTrigger (const Config* pConfig, ReadingClass& reader);
    ConfigTrigger (const Config* _pConfig, std::string _triggerName, const FrameSignalVariant _vpFrameSignal, unsigned long _compareConstValue, TriggerCompareOperator _compareOperator);
    const ConfigFrame*      getFrame() const;
    const ConfigSignal*     getSignal() const;
public:

    static bool             isSignalUsedForOperator(TriggerCompareOperator compareOperator);
    static bool             isConstCompareValueUsedForOperator(TriggerCompareOperator compareOperator);

    void                    setTriggerName(std::string triggerName);
    void                    setFrameSignalOperator(std::variant<const ConfigFrame*, const ConfigSignal*> _vpFrameSignal, ConfigTrigger::TriggerCompareOperator _oper);
    void                    setCompareConstValue(unsigned long value);

    std::string             getTriggerName() const;
    FrameSignalVariant      getFrameSignal() const;
    unsigned long           getCompareConstValue() const;
    TriggerCompareOperator  getCompareOperator() const;

public:
    bool operator==(const ConfigTrigger& b) const;

    //<----- ReadableFromBin, WritableToBin interfaces ----->//
    virtual void readFromBin(ReadingClass &reader) override;
    virtual void writeToBin(WritingClass &writer) override;

    ~ConfigTrigger() override {
    }
};

#endif // CONFIGTRIGGER_H
