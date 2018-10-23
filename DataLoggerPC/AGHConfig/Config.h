#ifndef CONFIG_H
#define CONFIG_H
#include "ReadingClass.h"
#include "WritingClass.h"
#include "ConfigFrame.h"
#include <vector>

using namespace std;

class Config : public WritableToBin, public ReadableFromBin {
private:
	unsigned int version;
	unsigned int subVersion;
    vector <ConfigFrame> frames;
public:
    Config();

	unsigned int get_version();
	unsigned int get_subVersion();
	unsigned int get_numOfFrames();

	void set_version(unsigned int sVersion);
	void set_subVersion(unsigned int sSubVersion);

	vector <ConfigFrame> get_frames();
	vector <ConfigFrame>::iterator get_frames_begin_iterator();
	vector <ConfigFrame>::iterator get_frames_end_iterator();
	void add_frame(ConfigFrame aChannel);

    void write_bin(WritingClass& writer) override;
    void read_bin(ReadingClass& reader) override;
};

#endif // CONFIG_H
