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
    int             multiplier;
    int             divider;
	int				offset;
	string			channelName;
	string			unitName;
	string			comment;
public:
    ConfigChannel();

    unsigned int    get_DLC() const ;
    ValueType       get_valueType() const ;
    int             get_multiplier() const ;
    int             get_divider() const ;
    int             get_offset() const ;
    string          get_channelName() const ;
    string          get_unitName() const ;
    string          get_comment() const ;

    void    set_valueType(ValueType);
    void    set_multiplier(int);
    void    set_divider(int);
    void    set_offset(int);
    void    set_channelName(string);
    void    set_unitName(string);
    void    set_comment(string);

    void    write_to_bin(WritingClass& writer) override;
    void    read_from_bin(ReadingClass& reader) override;

    bool    operator<(const ConfigChannel& rhs) const {return this->channelName < rhs.channelName;}
};

#endif // CONFIGCHANNEL_H
