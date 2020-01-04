#ifndef CONFIGSIGNAL_H
#define CONFIGSIGNAL_H
#include "AGHUtils/ReadingClass.h"
#include "AGHUtils/WritingClass.h"
#include "AGHConfig/ConfigFrame.h"
#include "ValueType.h"
#include "AGHConfig/ConfigSignalNamedValue.h"

#include <map>

using namespace std;

class ConfigFrame;

class ConfigSignal : public WritableToBin, public ReadableFromBin {
public:
    static constexpr unsigned int SIGNAL_MAX_LENGTH_BITS = 64;
private:
    ConfigFrame*                        parentFrame;

    ValueType                           valueType;
    unsigned int                        startBit;
    unsigned int                        lengthBits;
    int                                 multiplier;
    unsigned int                        divider;
    int                                 offset;
    string                              signalName;
    string                              unitName;
    string                              comment;
    map<int, ConfigSignalNamedValue>    channelNamedValues;

public:
    ConfigSignal(ConfigFrame* parentFrame);

    const ConfigFrame*    getParentFrame() const;

    ValueType       getValueType() const ;
    unsigned int    getLengthBits() const ;
    unsigned int    getStartBit() const;
    int             getMultiplier() const ;
    unsigned int    getDivider() const ;
    int             getOffset() const ;
    string          getSignalName() const ;
    string          getUnitName() const ;
    string          getComment() const ;

    void    setValueType(ValueType);
    void    setLengthBits(unsigned int lengthBits);
    void    setStartBit(unsigned int startBit);
    void    setMultiplier(int);
    void    setDivider(unsigned int);
    void    setOffset(int);
    void    setSignallName(string);
    void    setUnitName(string);
    void    setComment(string);

    bool                    hasNameForValue(int _channelValue);
    ConfigSignalNamedValue& getNamedValue(int channelValue);
    void                    addNamedValue(int _channelValue, ConfigSignalNamedValue _namedValue);

    void    writeToBin(WritingClass& writer) override;
    void    readFromBin(ReadingClass& reader) override;

    bool    operator<(const ConfigSignal& second) const {return this->signalName < second.signalName;}

    virtual ~ConfigSignal() override;
};

#endif // CONFIGSIGNAL_H
