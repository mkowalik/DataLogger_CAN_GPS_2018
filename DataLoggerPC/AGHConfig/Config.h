#ifndef CONFIG_H
#define CONFIG_H
#include "ReadingClass.h"
#include "WritingClass.h"
#include "ConfigFrame.h"
#include "ConfigChannel.h"
#include <vector>
#include <map>

using namespace std;

class Config : public WritableToBin, public ReadableFromBin {
public:
    enum class EnCANBitrate {
        bitrate_50kbps = 50,
        bitrate_125kbps = 125,
        bitrate_250kbps = 250,
        bitrate_500kbps = 500,
        bitrate_1Mbps = 1000,
    };
private:
    int             version;
    int             subVersion;
    EnCANBitrate    canBitrate;
    map <int, ConfigFrame> frames_map;
    static const int ACTUAL_VERSION = 0;
    static const int ACTUAL_SUB_VERSION = 2;
    static const EnCANBitrate DEFAULT_CAN_BITRATE = EnCANBitrate::bitrate_500kbps;
public:

    class iterator : public std::iterator<std::forward_iterator_tag, ConfigFrame> {
        friend class Config;
    private:
       map <int, ConfigFrame>::iterator innerIterator;
       Config& configReference;
    public:
       iterator(map <int, ConfigFrame>::iterator it, Config& configRef);
       bool operator==(const Config::iterator& second) const;
       bool operator!=(const Config::iterator& second) const;
       ConfigFrame& operator*();
       ConfigFrame* operator->();
       iterator& operator++();
       iterator operator++(int);
    };

    class const_iterator : public std::iterator<std::forward_iterator_tag, const ConfigFrame> {
        friend class Config;
    private:
       map <int, ConfigFrame>::const_iterator innerIterator;
       const Config& configReference;
    public:
       const_iterator(map <int, ConfigFrame>::const_iterator it, const Config& configRef);
       bool operator==(const Config::const_iterator& second) const;
       bool operator!=(const Config::const_iterator& second) const;
       const ConfigFrame& operator*();
       const ConfigFrame* operator->();
       const_iterator& operator++();
       const_iterator operator++(int);
    };

    static int get_actualVersion();
    static int get_actualSubVersion();

    Config();

    void            set_version(int sVersion);
    void            set_subVersion(int sSubVersion);
    void            set_CANBitrate(EnCANBitrate bitrate);

    int             get_version() const;
    int             get_subVersion() const;
    EnCANBitrate    get_CANBitrate() const;
    int             get_numOfFrames() const;
    ConfigFrame&    get_frame_by_id(int id);

    iterator        begin();
    iterator        end();
    const_iterator  cbegin();
    const_iterator  cend();

    bool            has_frame_with_id(int id) const;

    void            add_frame(ConfigFrame& aFrame);
    void            remove_frame_by_id(int id);
    void            reset();

    void            write_to_bin(WritingClass& writer) override;
    void            read_from_bin(ReadingClass& reader) override;


};


#endif // CONFIG_H
