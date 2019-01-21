#ifndef DATA_FILE_CLASS_H
#define DATA_FILE_CLASS_H

#include <fstream>
#include <map>
#include <set>
#include <ctime>
#include <functional>
#include "AGHConfig/ReadingClass.h"
#include "AGHConfig/Config.h"
#include "AGHData/raw_data_parser.h"

using namespace std;

/*****      SingleChannelData       *****/

class SingleChannelData {
    const ConfigChannel&    channel;
    int                     value;

public:
    SingleChannelData(const ConfigChannel& channel, int value);

    const ConfigChannel&    get_channel() const;
    int                     get_value_numeric() const;
    bool                    get_value_flag(int position) const;
};

/*****      DataRow       *****/

class SingleFrameData {
private:
    unsigned int                msTime;
    vector<SingleChannelData>   data;
public:
    SingleFrameData(unsigned int msTime, const ConfigFrame& frame, char rawValues[8], const RawDataParser& dataParser);

    unsigned int                        getMsTime() const;
    const vector<SingleChannelData>&    getData() const;
};

/*****      DataFileClass       *****/

class DataFileClass : public ReadableFromBin, public WritableToCSV {
private:
    Config                  config;
    tm                      startTime;
    vector<SingleFrameData> data;

    struct WrapperLess {
        bool operator()(const ConfigChannel& arg1, const ConfigChannel& arg2) const {return arg1 < arg2;}
    };

    void write_to_csv_static_period_mode(vector<reference_wrapper<const ConfigChannel>>& allChannelsVector, \
                                         map<reference_wrapper<const ConfigChannel>, int, WrapperLess>& channelValueMap,
                                         WritingClass& writer,
                                         unsigned int periodMs);
    void write_to_csv_event_mode(vector<reference_wrapper<const ConfigChannel>>& allChannelsVector, \
                                 map<reference_wrapper<const ConfigChannel>, int, WrapperLess>& channelValueMap,
                                 WritingClass& writer);
    void write_single_csv_data_row(unsigned int msTime, \
                                   const vector<reference_wrapper<const ConfigChannel>>& allChannelsVector, \
                                   const map<reference_wrapper<const ConfigChannel>, int, WrapperLess>& channelValueMap, \
                                   set<reference_wrapper<const ConfigChannel>, WrapperLess>& valueChangedSet, \
                                   WritingClass& writer);//TODO zamienic na unordered_set
public:

    const Config&                             get_config() const;
    tm                                        get_start_time() const;
    const vector<SingleFrameData>&            get_data() const;
    vector <SingleFrameData>::const_iterator  get_data_begin_iterator() const;
    vector <SingleFrameData>::const_iterator  get_data_end_iterator() const;

    void                                      add_data_row(SingleFrameData dataRow);

    void                                      write_to_csv(WritableToCSV::FileTimingMode mode, WritingClass& writer) override;
    void                                      read_from_bin(ReadingClass& reader) override;

};

#endif // DATA_FILE_CLASS_H
