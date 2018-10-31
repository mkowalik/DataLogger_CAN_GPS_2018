#ifndef CONFIG_H
#define CONFIG_H
#include "ReadingClass.h"
#include "WritingClass.h"
#include "ConfigFrame.h"
#include "ConfigChannel.h"
#include <vector>
#include <unordered_map>

using namespace std;

class Config : public WritableToBin, public ReadableFromBin, public WritableToCSV {
private:
    unsigned int            version;
    unsigned int            subVersion;
    vector<ConfigFrame>     frames;
    unordered_map <unsigned int, vector<ConfigFrame>::const_iterator> frames_map;
public:
    void                                set_version(unsigned int sVersion);
    void                                set_subVersion(unsigned int sSubVersion);

    unsigned int                        get_version() const;
    unsigned int                        get_subVersion() const;
    unsigned int                        get_numOfFrames() const;
    vector<ConfigFrame>::const_iterator get_frames_begin_iterator() const;
    vector<ConfigFrame>::const_iterator get_frames_end_iterator() const;
    void                                add_frame(ConfigFrame aFrame);
    ConfigFrame&                        get_frame_by_id(unsigned int id) const;
    vector<reference_wrapper<const ConfigChannel>> get_all_channels() const;

    void                                write_to_bin(WritingClass& writer) override;
    void                                write_to_csv(FileTimingMode mode, WritingClass& writer) override;
    void                                read_from_bin(ReadingClass& reader) override;

};

#endif // CONFIG_H
