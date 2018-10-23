#include "Config.h"
#include <iostream>
#include <algorithm>

using namespace std;

unsigned int Config::get_version(){
	return version;
}

unsigned int Config::get_subVersion(){
	return subVersion;
}

unsigned int Config::get_numOfFrames(){
	return frames.size();
}

void Config::set_version(unsigned int sVersion){
    version = min(sVersion, static_cast<unsigned int>(UINT16_MAX));
}
void Config::set_subVersion(unsigned int sSubVersion){
    subVersion = min(sSubVersion, static_cast<unsigned int>(UINT16_MAX));
}

vector <ConfigFrame> Config::get_frames(){
	return frames;
}

vector <ConfigFrame>::iterator Config::get_frames_begin_iterator(){
	return frames.begin();
}

vector <ConfigFrame>::iterator Config::get_frames_end_iterator(){
	return frames.end();
}

void Config::add_frame(ConfigFrame aChannel){
	frames.push_back(aChannel);
}

void Config::write_bin(WritingClass& writer){

    writer.write_uint16(get_version());
    writer.write_uint16(get_subVersion());
    writer.write_uint16(get_numOfFrames());

    for (vector<ConfigFrame>::iterator it=frames.begin(); it!=frames.end(); it++){
        it->write_bin(writer);
    }

}

void Config::read_bin(ReadingClass& reader){
    set_version(reader.reading_uint16());
    set_subVersion(reader.reading_uint16());
    unsigned int framesNumber = reader.reading_uint16();

    for(unsigned int i=0; i<framesNumber; ++i){
        ConfigFrame frame;
        frame.read_bin(reader);
        add_frame(frame);
    }
}


/*void Config::print(){
	std::cout << "Version: " << m_version;
	std::cout << "\tSubversion: " << m_subversion;
	std::cout << "\tNumber of frames: " << m_num_of_frames << std::endl << std::endl << std::endl;
	for (unsigned int i=0; i<m_frames.size(); i++){
		m_frames[i].print();
	}
}

void Config::writeToBinary(ofstream& data){
	data.write(reinterpret_cast<char*>(&m_version),sizeof(m_version));
	data.write(reinterpret_cast<char*>(&m_subversion),sizeof(m_subversion));
	data.write(reinterpret_cast<char*>(&m_num_of_frames),sizeof(m_num_of_frames));
	
	for(int i=0; i<m_num_of_frames; ++i){
		m_frames[i].writeToBinary(data);
	}

}
*/
