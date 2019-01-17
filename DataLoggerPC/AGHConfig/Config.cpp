#include "Config.h"
#include <iostream>
#include <algorithm>
#include <functional>

using namespace std;

unsigned int Config::get_version() const {
	return version;
}

unsigned int Config::get_subVersion() const {
	return subVersion;
}

unsigned int Config::get_numOfFrames() const {
	return frames.size();
}

void Config::set_version(unsigned int sVersion){
    version = min(sVersion, static_cast<unsigned int>(UINT16_MAX)); //TODO rzucić wyjątkiem
}

void Config::set_subVersion(unsigned int sSubVersion){
    subVersion = min(sSubVersion, static_cast<unsigned int>(UINT16_MAX)); //TODO rzucić wyjątkiem
}

vector <ConfigFrame>::const_iterator Config::get_frames_begin_citerator() const {
    return frames.cbegin();
}

vector <ConfigFrame>::const_iterator Config::get_frames_end_citerator() const {
    return frames.cend();
}

vector <ConfigFrame>::iterator Config::get_frames_begin_iterator() {
    return frames.begin();
}

vector <ConfigFrame>::iterator Config::get_frames_end_iterator() {
    return frames.end();
}

ConfigFrame& Config::get_frame_by_id(unsigned int id) {
    return const_cast<ConfigFrame&>(*(frames_map.at(id)));
}

ConfigFrame& Config::get_frame_by_position(unsigned int position) {
    return frames[position];
}

vector<reference_wrapper<const ConfigChannel>> Config::get_all_channels() const {

    vector<reference_wrapper<const ConfigChannel> > ret;

    for (vector <ConfigFrame>::const_iterator frameIt=get_frames_begin_citerator() ; frameIt!=get_frames_end_citerator(); frameIt++){
        for(vector <ConfigChannel>::const_iterator channelIt = frameIt->get_channels_begin_iterator(); channelIt!=frameIt->get_channels_end_iterator(); channelIt++){
            ret.push_back(ref(*channelIt));
        }
    }
    return ret;
}


void Config::add_frame(ConfigFrame aFrame){
    frames.push_back(aFrame);

    frames_map.insert({aFrame.get_ID(), frames.cend()-1});
}

void Config::write_to_bin(WritingClass& writer){

    writer.write_uint16(get_version());
    writer.write_uint16(get_subVersion());
    writer.write_uint16(get_numOfFrames());

    for (vector<ConfigFrame>::iterator it=frames.begin(); it!=frames.end(); it++){
        it->write_to_bin(writer);
    }

}

void Config::read_from_bin(ReadingClass& reader){
    set_version(reader.reading_uint16());
    set_subVersion(reader.reading_uint16());
    unsigned int framesNumber = reader.reading_uint16();

    for(unsigned int i=0; i<framesNumber; ++i){
        ConfigFrame frame;
        frame.read_from_bin(reader);
        add_frame(frame);
    }
}

void Config::write_to_csv(FileTimingMode mode, WritingClass& writer){

    vector<reference_wrapper<const ConfigChannel> > allChannels = get_all_channels();

    for(vector<reference_wrapper<const ConfigChannel> >::iterator channelIt = allChannels.begin(); \
        channelIt!=allChannels.end(); \
        channelIt++){

        const ConfigChannel& tmp = *channelIt;
        writer.write_string(tmp.get_channelName() + " [" + tmp.get_unitName() + "]");
        writer.write_string(";");
    }
    writer.write_string("\r\n");

}
