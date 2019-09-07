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
    enum class EnGPSFrequency {
        freq_GPS_OFF = 0,
        freq_0_5_Hz  = 1,
        freq_1_Hz    = 2,
        freq_2_Hz    = 3,
        freq_5_Hz    = 4,
        freq_10_Hz   = 5
    };

private:
    int             version;
    int             subVersion;
    EnCANBitrate    canBitrate;
    EnGPSFrequency  gpsFrequency;
    map <int, ConfigFrame> frames_map;
    static const int ACTUAL_VERSION = 0;
    static const int ACTUAL_SUB_VERSION = 3;
    static const EnCANBitrate DEFAULT_CAN_BITRATE = EnCANBitrate::bitrate_500kbps;
    static const EnGPSFrequency DEFAULT_GPS_FREQUENCY = EnGPSFrequency::freq_10_Hz;
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
    void            set_GPSFrequency(EnGPSFrequency frequency);

    int             get_version() const;
    int             get_subVersion() const;
    EnCANBitrate    get_CANBitrate() const;
    EnGPSFrequency  get_GPSFrequency() const;
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
