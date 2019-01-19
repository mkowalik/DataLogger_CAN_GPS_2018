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
    int                     ID;
    string                  moduleName;
    vector <ConfigChannel>  channels;
public:
    int                     get_ID() const;
    int                     get_DLC() const;
    string                  get_moduleName() const;

    void                    set_ID(int aID);
    void                    set_moduleName(string aModuleName);

    vector <ConfigChannel>::const_iterator get_channels_begin_iterator() const;
    vector <ConfigChannel>::const_iterator get_channels_end_iterator() const;
    ConfigChannel&          get_channel_by_position(int position);
    void                    add_channel(ConfigChannel aChannel);
    void                    remove_channel_by_position(int position);

    void                    write_to_bin(WritingClass& writer) override;
    void                    read_from_bin(ReadingClass& reader) override;
};

#endif // CONFIGFRAME_H
