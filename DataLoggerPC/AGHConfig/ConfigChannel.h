#ifndef CONFIGCHANNEL_H
#define CONFIGCHANNEL_H
#include "ReadingClass.h"
#include "WritingClass.h"
#include "ValueType.h"
#define SIZE 20

using namespace std;

class ConfigChannel : public WritableToBin, public ReadableFromBin {
private:
	ValueType		valueType;
	unsigned int	multiplier;
	unsigned int	divider;
	int				offset;
	string			channelName;
	string			unitName;
	string			comment;
public:
    ConfigChannel();
	unsigned int get_DLC();

	ValueType get_valueType();
	unsigned int get_multiplier();
	unsigned int get_divider();
	int get_offset();
	string get_channelName();
	string get_unitName();
	string get_comment();

	void set_valueType(ValueType);
	void set_multiplier(unsigned int);
	void set_divider(unsigned int);
	void set_offset(int);
	void set_channelName(string);
	void set_unitName(string);
	void set_comment(string);

    void write_bin(WritingClass& writer) override;
    void read_bin(ReadingClass& reader) override;
};

#endif // CONFIGCHANNEL_H
