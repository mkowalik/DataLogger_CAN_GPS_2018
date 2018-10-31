#include "data_file_class.h"
#include "raw_data_parser.h"

using namespace std;

/***************      SingleChannelData       ***************/

SingleChannelData::SingleChannelData(const ConfigChannel& channel, int value) : channel(channel), value(value){
}

const ConfigChannel& SingleChannelData::get_channel() const{
    return channel;
}
int SingleChannelData::get_value() const{
    return value;
}

/***************      DataRow       ***************/

SingleFrameData::SingleFrameData(unsigned int msTime, const ConfigFrame& frame, char rawValues[8], const RawDataParser& dataParser) : msTime(msTime){

    int i=0;
    for (vector<ConfigChannel>::const_iterator it=frame.get_channels_begin_iterator(); it!=frame.get_channels_end_iterator(); it++){
        int value = dataParser.interpret_signed_int(rawValues+i, it->get_DLC());
        data.push_back(SingleChannelData(*it, value));
    }

};

const vector<SingleChannelData>& SingleFrameData::getData() const {
    return data;
}

unsigned int SingleFrameData::getMsTime() const {
    return msTime;
}

/***************      DataFileClass       ***************/

const Config& DataFileClass::get_config() const {
    return const_cast<Config&>(config);
}

tm DataFileClass::get_start_time() const {
    return startTime;
}

const vector<SingleFrameData>& DataFileClass::get_data() const {
    return const_cast<vector<SingleFrameData>&>(data);
}

vector <SingleFrameData>::const_iterator DataFileClass::get_data_begin_iterator() const {
    return data.cbegin();
}
vector <SingleFrameData>::const_iterator DataFileClass::get_data_end_iterator() const {
    return data.cend();
}

void DataFileClass::add_data_row(SingleFrameData dataRow) {
    data.push_back(dataRow);
}

void DataFileClass::write_single_csv_data_row(unsigned int msTime, \
                                              const vector<reference_wrapper<const ConfigChannel>>& allChannelsVector, \
                                              const map<reference_wrapper<const ConfigChannel>, int, WrapperLess>& channelValueMap, \
                                              set<reference_wrapper<const ConfigChannel>, WrapperLess>& valueChangedSet, \
                                              WritingClass& writer){//TODO zamienic na unordered_set

    set<reference_wrapper<const ConfigChannel>, WrapperLess>::iterator changedIt;

    writer.write_string(to_string(msTime));

    for (auto&& channelWrap : allChannelsVector){
        writer.write_string(";");
        if ((changedIt = valueChangedSet.find(channelWrap)) != valueChangedSet.end()){
            writer.write_string(to_string(channelValueMap.at(channelWrap)));
            valueChangedSet.erase(changedIt);
        }
    }
}


void DataFileClass::write_to_csv_static_period_mode(vector<reference_wrapper<const ConfigChannel>>& allChannelsVector, \
                                                    map<reference_wrapper<const ConfigChannel>, int, WrapperLess>& channelValueMap,
                                                    WritingClass& writer,
                                                    unsigned int periodMs) {
    unsigned int actualMsTime = 0;

    set<reference_wrapper<const ConfigChannel>, WrapperLess> valueChangedSet;

    for (auto&& row : data){
        if (row.getMsTime() > actualMsTime + periodMs){
            write_single_csv_data_row(actualMsTime,
                                      allChannelsVector,
                                      channelValueMap,
                                      valueChangedSet,
                                      writer);
            actualMsTime += periodMs;
        }
        for(auto&& singleData : row.getData()){
            channelValueMap[singleData.get_channel()] = singleData.get_value();
            valueChangedSet.insert(singleData.get_channel());
        }
    }

    write_single_csv_data_row(actualMsTime + periodMs,
                              allChannelsVector,
                              channelValueMap,
                              valueChangedSet,
                              writer);
}

void DataFileClass::write_to_csv_event_mode(vector<reference_wrapper<const ConfigChannel>>& allChannelsVector, \
                                            map<reference_wrapper<const ConfigChannel>, int, WrapperLess>& channelValueMap,
                                            WritingClass& writer) {

    set<reference_wrapper<const ConfigChannel>, WrapperLess> valueChangedSet;

    for (auto&& row : data){
        for(auto&& singleData : row.getData()){
            channelValueMap[singleData.get_channel()] = singleData.get_value();
            valueChangedSet.insert(singleData.get_channel());
        }
        write_single_csv_data_row(row.getMsTime(),
                                  allChannelsVector,
                                  channelValueMap,
                                  valueChangedSet,
                                  writer);
    }

}

void DataFileClass::write_to_csv(WritableToCSV::FileTimingMode mode, WritingClass& writer) {

    config.write_to_csv(mode, writer);

    map<reference_wrapper<const ConfigChannel>, int, WrapperLess>   channelValueMap; //TODO dobrze byłoby zamienić na unordered_map, ale trzeba dorzucic ten hash :-(
    vector<reference_wrapper<const ConfigChannel>>                  allChannelsVector = config.get_all_channels();

    for(auto channelIt = allChannelsVector.cbegin(); channelIt!=allChannelsVector.cend(); channelIt++){
        channelValueMap.insert(pair<reference_wrapper<const ConfigChannel>, int>(*channelIt, 0));
    }

    if (mode == WritableToCSV::EventMode){
        write_to_csv_event_mode(allChannelsVector, channelValueMap, writer);
    } else {
        switch(mode){
        case StaticPeriod10HzMode:
            write_to_csv_static_period_mode(allChannelsVector, channelValueMap, writer, 100);
            break;
        case StaticPeriod100HzMode:
            write_to_csv_static_period_mode(allChannelsVector, channelValueMap, writer, 10);
            break;
        case StaticPeriod250HzMode:
            write_to_csv_static_period_mode(allChannelsVector, channelValueMap, writer, 4);
            break;
        case StaticPeriod500HzMode:
            write_to_csv_static_period_mode(allChannelsVector, channelValueMap, writer, 2);
            break;
        case StaticPeriod1000HzMode:
            write_to_csv_static_period_mode(allChannelsVector, channelValueMap, writer, 1);
            break;
        default:
            throw "No such mode exception!"; //TODO
        }
    }

}

void DataFileClass::read_from_bin(ReadingClass& reader) {

    config.read_from_bin(reader);

    startTime.tm_year = static_cast<int>(reader.reading_uint16());
    startTime.tm_mon  = static_cast<int>(reader.reading_uint8());
    startTime.tm_mday = static_cast<int>(reader.reading_uint8());
    startTime.tm_hour = static_cast<int>(reader.reading_uint8());
    startTime.tm_min  = static_cast<int>(reader.reading_uint8());
    startTime.tm_sec  = static_cast<int>(reader.reading_uint8());

    while(reader.eof()){
        char            buffer[8];
        unsigned int    msTime   = reader.reading_uint32();
        unsigned int    frameID  = reader.reading_uint16();
        ConfigFrame& configFrame = config.get_frame_by_id(frameID);
        reader.reading_bytes(buffer, configFrame.get_DLC());
        SingleFrameData dataRow(msTime, configFrame, buffer, reader.get_dataParser());

        data.push_back(dataRow);
    }

}
