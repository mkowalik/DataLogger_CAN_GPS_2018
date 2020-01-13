#ifndef CONFIGSIGNAL_H
#define CONFIGSIGNAL_H

#include <map>

#include "AGHUtils/ReadingClass.h"
#include "AGHUtils/WritingClass.h"

#include "AGHConfig/ConfigSignalNamedValue.h"
#include "AGHConfig/ValueType.h"

using namespace std;

class ConfigFrame;

class ConfigSignal : public WritableToBin, public ReadableFromBin {
    friend class ConfigFrame;
public:
    static constexpr unsigned int SIGNAL_MAX_LENGTH_BITS = 64;
    static constexpr unsigned int SIGNAL_ID_MAX_VALUE    = UINT16_MAX;
private:
    ConfigSignal(ConfigFrame* pParentFrame, ReadingClass& reader);
    ConfigSignal(ConfigFrame*   pParentFrame,
                 unsigned int   signalID,
                 unsigned int   startBit,
                 unsigned int   lengthBits,
                 ValueType      valueType,
                 int            multiplier,
                 unsigned int   divider,
                 int            offset,
                 string         signalName,
                 string         unitName,
                 string         comment
    );
    void    _setSignalId(unsigned int signalID);


    ConfigFrame*                     pParentFrame;

    unsigned int                     signalID;

    unsigned int                     startBit;
    unsigned int                     lengthBits;
    ValueType                        valueType;
    int                              multiplier;
    unsigned int                     divider;
    int                              offset;
    string                           signalName;
    string                           unitName;
    string                           comment;

    map<int, ConfigSignalNamedValue> channelNamedValues; //TODO to be implemented in the future

public:

    ConfigFrame*        getParentFrame() const;
    unsigned int        getSignalID() const;
    unsigned int        getStartBit() const;
    unsigned int        getLengthBits() const;
    ValueType           getValueType() const;
    int                 getMultiplier() const;
    unsigned int        getDivider() const;
    int                 getOffset() const;
    string              getSignalName() const;
    string              getUnitName() const;
    string              getComment() const;

    void                setSignalID(unsigned int signalID);
    void                setLengthBits(unsigned int lengthBits);
    void                setStartBit(unsigned int startBit);
    void                setValueType(ValueType);
    void                setMultiplier(int);
    void                setDivider(unsigned int);
    void                setOffset(int);
    void                setSignallName(string);
    void                setUnitName(string);
    void                setComment(string);

    bool                    hasNameForValue(int _channelValue);
    ConfigSignalNamedValue& getNamedValue(int channelValue);
    void                    addNamedValue(int _channelValue, ConfigSignalNamedValue _namedValue);

    virtual void            writeToBin(WritingClass& writer) override;
    virtual void            readFromBin(ReadingClass& reader) override;

    virtual ~ConfigSignal() override;
};

#endif // CONFIGSIGNAL_H
