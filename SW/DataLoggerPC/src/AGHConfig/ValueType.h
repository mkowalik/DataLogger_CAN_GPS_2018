#ifndef VALUETYPE_H
#define VALUETYPE_H

#include "AGHUtils/WritingClass.h"
#include "AGHUtils/ReadingClass.h"

class ValueType : public WritableToBin, public ReadableFromBin {
private:
    union {
        struct {
            unsigned char _unused : 7;
            unsigned char isSigned : 1;
        } s;
        unsigned char b;
    } byteValue;
public:
    ValueType();
    ValueType(unsigned char _byteValue);
    ValueType(bool setSigned, bool set16bit, bool setOnOffType, bool setFlagType, bool setCustomType, bool setBigEndianType);
    bool            isSignedType() const;
    bool            is16BitLength() const ;
    bool            isOnOffType() const;
    bool            isFlagType() const;
    bool            isCustomType() const;
    bool            isBigEndianType() const;

    unsigned char   getByteValue() const;

    void            writeToBin(WritingClass& writer) override;
    void            readFromBin(ReadingClass& reader) override;
};

#endif // VALUETYPE_H
