#ifndef VALUETYPE_H
#define VALUETYPE_H

#include "WritingClass.h"
#include "ReadingClass.h"

class ValueType : public WritableToBin, public ReadableFromBin {
private:
    unsigned int feature;
public:
	ValueType(unsigned char feature);
    unsigned int    channelDLC() const;
    bool            isSignedType() const;
    bool            is16BitLength() const ;
    bool            isOnOffType() const;
    bool            isFlagType() const;
    bool            isCustomType() const;

    void            write_to_bin(WritingClass& writer) override;
    void            read_from_bin(ReadingClass& reader) override;
};

#endif // VALUETYPE_H
