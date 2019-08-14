#ifndef VALUETYPE_H
#define VALUETYPE_H

#include "WritingClass.h"
#include "ReadingClass.h"

class ValueType : public WritableToBin, public ReadableFromBin {
private:
    unsigned int feature;
public:
    ValueType();
	ValueType(unsigned char feature);
    ValueType(bool setSigned, bool set16bit, bool setOnOffType, bool setFlagType, bool setCustomType, bool setBigEndianType);
    int             channelDLC() const;
    bool            isSignedType() const;
    bool            is16BitLength() const ;
    bool            isOnOffType() const;
    bool            isFlagType() const;
    bool            isCustomType() const;
    bool            isBigEndianType() const;

    unsigned char   getHexValue() const;

    void            write_to_bin(WritingClass& writer) override;
    void            read_from_bin(ReadingClass& reader) override;
};

#endif // VALUETYPE_H
