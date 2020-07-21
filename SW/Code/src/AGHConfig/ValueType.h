#ifndef VALUETYPE_H
#define VALUETYPE_H

#include "AGHUtils/WritingClass.h"
#include "AGHUtils/ReadingClass.h"

class ValueType : public WritableToBin, public ReadableFromBin {
private:
    union ByteValueType {
        struct {
            unsigned char isBigEndian : 1;
            unsigned char isSigned : 1;
            unsigned char _unused : 6;
        } s;
        unsigned char b;
    } byteValue;
    void            setByteValue(unsigned char _byteValue);
public:
    ValueType(unsigned char _byteValue);
    ValueType(bool setSigned, bool setBigEndianType);

    bool            isSignedType() const;
    bool            isBigEndianType() const;

    unsigned char   getByteValue() const;

    virtual void    writeToBin(WritingClass& writer) override;
    virtual void    readFromBin(ReadingClass& reader) override;
};

#endif // VALUETYPE_H
