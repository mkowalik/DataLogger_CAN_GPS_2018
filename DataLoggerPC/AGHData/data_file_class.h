#ifndef DATA_FILE_CLASS_H
#define DATA_FILE_CLASS_H

#include <fstream>
#include <map>
#include <set>
#include <ctime>
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
        bool operator()(const ConfigChannel* arg1, const ConfigChannel* arg2) const {return arg1 < arg2;}
    };

    void write_to_csv_static_period_mode(map<const ConfigChannel*, int, WrapperLess>& channelValueMap,
                                         WritingClass& writer,
                                         unsigned int periodMs,
                                         char decimalSeparator);
    void write_to_csv_event_mode(map<const ConfigChannel*, int, WrapperLess>& channelValueMap,
                                 WritingClass& writer, char decimalSeparator);
    void write_single_csv_data_row(unsigned int msTime, \
                                   const map<const ConfigChannel*, int, WrapperLess>& channelValueMap, \
                                   set<const ConfigChannel*, WrapperLess>& valueChangedSet, \
                                   char decimalSeparator, \
                                   WritingClass& writer);
public:

    class iterator : public std::iterator<std::forward_iterator_tag, SingleFrameData> {
        friend class DataFileClass;
    private:
       vector<SingleFrameData>::iterator innerIterator;
       DataFileClass& dataFileReference;
    public:
       iterator(vector<SingleFrameData>::iterator it, DataFileClass& dataFileRef);
       bool operator==(const DataFileClass::iterator& second) const;
       bool operator!=(const DataFileClass::iterator& second) const;
       SingleFrameData& operator*();
       SingleFrameData* operator->();
       iterator& operator++();
       iterator operator++(int);
    };

    class const_iterator : public std::iterator<std::forward_iterator_tag, SingleFrameData> {
        friend class DataFileClass;
    private:
       vector<SingleFrameData>::const_iterator innerIterator;
       const DataFileClass& dataFileReference;
    public:
       const_iterator(vector<SingleFrameData>::const_iterator it, const DataFileClass& dataFileRef);
       bool operator==(const DataFileClass::const_iterator& second) const;
       bool operator!=(const DataFileClass::const_iterator& second) const;
       const SingleFrameData& operator*();
       const SingleFrameData* operator->();
       const_iterator& operator++();
       const_iterator operator++(int);
    };

    const Config&                       get_config() const;
    tm                                  get_start_time() const;
    const vector<SingleFrameData>&      get_data() const;

    void                                append_data_row(SingleFrameData dataRow);

    iterator                            begin();
    iterator                            end();
    const_iterator                      cbegin() const;
    const_iterator                      cend() const;


    void                                write_to_csv(WritableToCSV::FileTimingMode mode, WritingClass& writer, char decimalSeparator) override;
    void                                read_from_bin(ReadingClass& reader) override;

};

#endif // DATA_FILE_CLASS_H
