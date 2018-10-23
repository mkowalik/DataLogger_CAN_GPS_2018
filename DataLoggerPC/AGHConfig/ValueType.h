#ifndef VALUETYPE_H
#define VALUETYPE_H

#include "WritingClass.h"
#include "ReadingClass.h"

class ValueType : public WritableToBin, public ReadableFromBin {
private:
    unsigned int feature;
public:
	ValueType(unsigned char feature);
	unsigned int channelDLC();
	bool isSignedType();
	bool is16BitLength();
	bool isOnOffType();
	bool isFlagType();
	bool isCustomType();
    void write_bin(WritingClass& writer) override;
    void read_bin(ReadingClass& reader) override;
};

#endif // VALUETYPE_H
