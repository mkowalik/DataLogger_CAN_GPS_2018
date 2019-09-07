#ifndef DATA_FILE_CLASS_H
#define DATA_FILE_CLASS_H

#include <fstream>
#include <map>
#include <set>
#include <ctime>

#include "AGHConfig/ReadingClass.h"
#include "AGHConfig/Config.h"
#include "AGHData/raw_data_parser.h"
#include "AGHData/fixed_point.h"
#include "AGHData/single_can_frame_data.h"
#include "AGHData/single_gps_frame_data.h"
#include "AGHData/can_channel_with_last_value.h"

using namespace std;

/*****      DataFileClass       *****/

class DataFileClass : public ReadableFromBin, public WritableToCSV {
private:
    static constexpr int       GPS_DATA_ID = 0x800;
    static constexpr int       GPS_DATA_FIELDS_NUMBER = 10;
    static constexpr int       DEFAULT_CHANNEL_RAW_VALUE = 0;
    static constexpr double    DEFAULT_CHANNEL_TRANSFORMED_VALUE = 0.0;
    Config                     config;
    tm                         startTime;
    vector<SingleCANFrameData> canData;
    vector<SingleGPSFrameData> gpsData;
    char                       decimalSeparator;

    void write_single_csv_data_row(unsigned int msTime,
                                   WritingClass& writer,
                                   const SingleGPSFrameData* pGpsData,
                                   vector<map<int, vector <CANChannelWithLastValue>>::iterator>& csv_frames_columns_order,
                                   bool writeOnlyChangedValues);
    void write_single_gps_row(WritingClass& writer, const SingleGPSFrameData* pGpsData);

    void write_to_csv_static_period_mode(WritingClass& writer,
                                         map<int, vector <CANChannelWithLastValue>>& lastValues,
                                         const SingleGPSFrameData* pGpsData,
                                         vector<map<int, vector <CANChannelWithLastValue>>::iterator>& csv_frames_columns_order,
                                         bool writeOnlyChangedValues,
                                         unsigned int periodMs) ;
    void write_to_csv_event_mode(WritingClass& writer,
                                 map<int, vector <CANChannelWithLastValue>>& lastValues,
                                 const SingleGPSFrameData* pGpsData,
                                 vector<map<int, vector <CANChannelWithLastValue>>::iterator>& csv_frames_columns_order,
                                 bool writeOnlyChangedValues);
    void write_to_csv_frame_by_frame_mode(WritingClass& writer);
public:

    class iterator : public std::iterator<std::forward_iterator_tag, SingleCANFrameData> {
        friend class DataFileClass;
    private:
       vector<SingleCANFrameData>::iterator innerIterator;
       DataFileClass& dataFileReference;
    public:
       iterator(vector<SingleCANFrameData>::iterator it, DataFileClass& dataFileRef);
       bool operator==(const DataFileClass::iterator& second) const;
       bool operator!=(const DataFileClass::iterator& second) const;
       SingleCANFrameData& operator*();
       SingleCANFrameData* operator->();
       iterator& operator++();
       iterator operator++(int);
    };

    class const_iterator : public std::iterator<std::forward_iterator_tag, SingleCANFrameData> {
        friend class DataFileClass;
    private:
       vector<SingleCANFrameData>::const_iterator innerIterator;
       const DataFileClass& dataFileReference;
    public:
       const_iterator(vector<SingleCANFrameData>::const_iterator it, const DataFileClass& dataFileRef);
       bool operator==(const DataFileClass::const_iterator& second) const;
       bool operator!=(const DataFileClass::const_iterator& second) const;
       const SingleCANFrameData& operator*();
       const SingleCANFrameData* operator->();
       const_iterator& operator++();
       const_iterator operator++(int);
    };

    const Config&                       get_config() const;
    tm                                  get_start_time() const;
    const vector<SingleCANFrameData>&   get_data() const;

    void                                append_data_row(SingleCANFrameData dataRow);

    void                                write_to_csv(WritableToCSV::FileTimingMode mode, WritingClass& writer, char decimalSeparator, bool writeOnlyChangedValues) override;
    void                                read_from_bin(ReadingClass& reader) override;

    iterator                            begin();
    iterator                            end();
    const_iterator                      cbegin() const;
    const_iterator                      cend() const;

};

#endif // DATA_FILE_CLASS_H
