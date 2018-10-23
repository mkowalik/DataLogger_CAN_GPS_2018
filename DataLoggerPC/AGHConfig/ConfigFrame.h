#ifndef CONFIGFRAME_H
#define CONFIGFRAME_H
#include "ReadingClass.h"
#include "WritingClass.h"
#include "ConfigChannel.h"
#include <vector>
#include <fstream>

using namespace std;

class ConfigFrame : public WritableToBin, public ReadableFromBin {
private:
	unsigned int	ID;
	string			moduleName;
	vector <ConfigChannel> channels;
public:
    ConfigFrame();

	unsigned int get_ID();
	unsigned int get_DLC();
	string get_moduleName();

    void set_ID(unsigned int aID);
	void set_moduleName(string aModuleName);

	vector <ConfigChannel> get_channels();
	vector <ConfigChannel>::iterator get_channels_begin_iterator();
	vector <ConfigChannel>::iterator get_channels_end_iterator();
	void add_channel(ConfigChannel aChannel);

    void write_bin(WritingClass& writer) override;
    void read_bin(ReadingClass& reader) override;
};

#endif // CONFIGFRAME_H
