#include "data_file_class.h"
#include "raw_data_parser.h"
#include <cstring>

using namespace std;

/***************      SingleChannelData       ***************/

SingleChannelData::SingleChannelData(const ConfigChannel& channel, int value) : channel(channel), value(value){
}

const ConfigChannel& SingleChannelData::get_channel() const{
    return channel;
}
int SingleChannelData::get_value_numeric() const{
    return value;
}
/***************      DataRow       ***************/

SingleFrameData::SingleFrameData(unsigned int msTime, const ConfigFrame& frame, char rawValues[8], const RawDataParser& dataParser) : msTime(msTime){

    int i=0;
    for (ConfigFrame::const_iterator it=frame.cbegin(); it!=frame.cend(); ++it){
        int value = dataParser.interpret_signed_int(rawValues+i, it->get_DLC());
        data.push_back(SingleChannelData(*it, value));
        i += it->get_DLC();
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

void DataFileClass::append_data_row(SingleFrameData dataRow) {
    data.push_back(dataRow);
}

DataFileClass::iterator DataFileClass::begin(){
    return iterator(data.begin(), *this);
}

DataFileClass::iterator DataFileClass::end(){
    return iterator(data.end(), *this);
}

DataFileClass::const_iterator DataFileClass::cbegin() const {
    return const_iterator(data.cbegin(), *this);
}

DataFileClass::const_iterator DataFileClass::cend() const {
    return const_iterator(data.cend(), *this);
}

void DataFileClass::write_single_csv_data_row(unsigned int msTime, \
                                              const map<const ConfigChannel*, int, WrapperLess>& channelValueMap, \
                                              set<const ConfigChannel*, WrapperLess>& valueChangedSet, \
                                              char decimalSeparator, \
                                              WritingClass& writer){ //TODO zamienic na unordered_set

    set<const ConfigChannel*, WrapperLess>::iterator changedIt;

    writer.write_string(to_string(msTime), false);

    for(Config::const_iterator frIt = config.cbegin(); frIt != config.cend(); ++frIt){
        for (ConfigFrame::const_iterator chIt = frIt->cbegin(); chIt != frIt->cend(); ++chIt){
            writer.write_string(";", false);
            if ((changedIt = valueChangedSet.find(&(*chIt))) != valueChangedSet.end()){

                if (chIt->get_divider() != 1){
                    double value = (((static_cast<double>(channelValueMap.at(&(*chIt)))) * chIt->get_multiplier()) / chIt->get_divider()) + chIt->get_offset();
                    writer.write_double_to_string(value, 2, decimalSeparator, false);
                } else {
                    writer.write_int_to_string((channelValueMap.at(&(*chIt)) * chIt->get_multiplier()) + chIt->get_offset(), false);
                }
                valueChangedSet.erase(changedIt);
            }
        }
    }
    writer.write_string("\r\n", false);
}


void DataFileClass::write_to_csv_static_period_mode(map<const ConfigChannel*, int, WrapperLess>& channelValueMap,
                                                    WritingClass& writer,
                                                    unsigned int periodMs,
                                                    char decimalSeparator) {
    unsigned int actualMsTime = 0;

    set<const ConfigChannel*, WrapperLess> valueChangedSet;

    for (auto&& row : data){
        if (row.getMsTime() > actualMsTime + periodMs){
            write_single_csv_data_row(actualMsTime,
                                      channelValueMap,
                                      valueChangedSet,
                                      decimalSeparator,
                                      writer);
            actualMsTime += periodMs;
        }
        for(auto& singleData : row.getData()){
            channelValueMap[&singleData.get_channel()] = singleData.get_value_numeric();
            valueChangedSet.insert(&singleData.get_channel());
        }
    }

    write_single_csv_data_row(actualMsTime + periodMs,
                              channelValueMap,
                              valueChangedSet,
                              decimalSeparator,
                              writer);
}

void DataFileClass::write_to_csv_event_mode(map<const ConfigChannel*, int, WrapperLess>& channelValueMap,
                                            WritingClass& writer, char decimalSeparator) {

    set<const ConfigChannel*, WrapperLess> valueChangedSet;

    for (auto& row : data){
        for(auto& singleData : row.getData()){
            const ConfigChannel* tmp = &singleData.get_channel();
            channelValueMap[tmp] = singleData.get_value_numeric();
            valueChangedSet.insert(tmp);
        }
        write_single_csv_data_row(row.getMsTime(),
                                  channelValueMap,
                                  valueChangedSet,
                                  decimalSeparator,
                                  writer);
    }

}

static const int defaultChannelValue = 0;

void DataFileClass::write_to_csv(WritableToCSV::FileTimingMode mode, WritingClass& writer, char decimalSeparator) {

    map<const ConfigChannel*, int, WrapperLess> channelValueMap; //TODO dobrze byłoby zamienić na unordered_map, ale trzeba dorzucic ten hash :-(

    /*************  Preparing map witch channels as keys and int values   *************/

    for(Config::const_iterator frIt = config.cbegin(); frIt != config.cend(); ++frIt){
        for (ConfigFrame::const_iterator chIt = frIt->cbegin(); chIt != frIt->cend(); ++chIt){
            channelValueMap.insert(pair<const ConfigChannel*, int>(&(*chIt), defaultChannelValue));
        }
    }

    /*************  Writing down colums with time and channels   *************/

    writer.write_string("time [ms];", false);

    for(Config::const_iterator frIt = config.cbegin(); frIt != config.cend(); ++frIt){
        for (ConfigFrame::const_iterator chIt = frIt->cbegin(); chIt != frIt->cend(); ++chIt){
            writer.write_string(chIt->get_channelName() + " [" + chIt->get_unitName() + "]", false);
            writer.write_string(";", false);
        }
    }

    writer.write_string("\r\n", false);


    /*************  Writing down data rows   *************/

    if (mode == WritableToCSV::EventMode){
        write_to_csv_event_mode(channelValueMap, writer, decimalSeparator);
    } else {
        switch(mode){
        case StaticPeriod10HzMode:
            write_to_csv_static_period_mode(channelValueMap, writer, 100, decimalSeparator);
            break;
        case StaticPeriod100HzMode:
            write_to_csv_static_period_mode(channelValueMap, writer, 10, decimalSeparator);
            break;
        case StaticPeriod250HzMode:
            write_to_csv_static_period_mode(channelValueMap, writer, 4, decimalSeparator);
            break;
        case StaticPeriod500HzMode:
            write_to_csv_static_period_mode(channelValueMap, writer, 2, decimalSeparator);
            break;
        case StaticPeriod1000HzMode:
            write_to_csv_static_period_mode(channelValueMap, writer, 1, decimalSeparator);
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

    while(!reader.eof()){
        char            buffer[8];
        unsigned int    msTime   = reader.reading_uint32();
        int             frameID  = static_cast<int>(reader.reading_uint16());
        ConfigFrame& configFrame = config.get_frame_by_id(frameID);
        reader.reading_bytes(buffer, configFrame.get_DLC());
        SingleFrameData dataRow(msTime, configFrame, buffer, reader.get_dataParser());

        data.push_back(dataRow);
    }
}

DataFileClass::iterator::iterator(vector<SingleFrameData>::iterator it, DataFileClass& dataFileRef) :
        innerIterator(it), dataFileReference(dataFileRef)
{
}

bool DataFileClass::iterator::operator==(const DataFileClass::iterator& second) const {
    return (innerIterator == second.innerIterator);
}

bool DataFileClass::iterator::operator!=(const DataFileClass::iterator &second) const {
    return (innerIterator != second.innerIterator);
}

SingleFrameData& DataFileClass::iterator::operator*(){
    return (*innerIterator);
}

SingleFrameData* DataFileClass::iterator::operator->(){
    return &(*innerIterator);
}

DataFileClass::iterator& DataFileClass::iterator::operator++(){
    ++innerIterator;
    return (*this);
}

DataFileClass::iterator DataFileClass::iterator::operator++(int){
    iterator ret(*this);
    ++innerIterator;
    return ret;
}

DataFileClass::const_iterator::const_iterator(vector<SingleFrameData>::const_iterator it, const DataFileClass& dataFileRef) :
        innerIterator(it), dataFileReference(dataFileRef) {
}

bool DataFileClass::const_iterator::operator==(const DataFileClass::const_iterator& second) const {
    return (innerIterator == second.innerIterator);
}

bool DataFileClass::const_iterator::operator!=(const DataFileClass::const_iterator &second) const {
    return (innerIterator != second.innerIterator);
}

const SingleFrameData& DataFileClass::const_iterator::operator*() {
    return (*innerIterator);
}

const SingleFrameData* DataFileClass::const_iterator::operator->() {
    return &(*innerIterator);
}

DataFileClass::const_iterator& DataFileClass::const_iterator::operator++(){
    ++innerIterator;
    return (*this);
}

DataFileClass::const_iterator DataFileClass::const_iterator::operator++(int){
    const_iterator ret(*this);
    ++innerIterator;
    return ret;
}
