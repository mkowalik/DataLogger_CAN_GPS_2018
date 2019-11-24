#ifndef CONFIG_H
#define CONFIG_H
#include "AGHUtils/ReadingClass.h"
#include "AGHUtils/WritingClass.h"
#include "ConfigFrame.h"
#include "ConfigSignal.h"
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
    int                              version;
    int                              subVersion;
    EnCANBitrate                     canBitrate;
    EnGPSFrequency                   gpsFrequency;
    map <unsigned int, ConfigFrame*> idToFrameMap;

    static const int            ACTUAL_VERSION = 0;
    static const int            ACTUAL_SUB_VERSION = 3;
    static const EnCANBitrate   DEFAULT_CAN_BITRATE = EnCANBitrate::bitrate_500kbps;
    static const EnGPSFrequency DEFAULT_GPS_FREQUENCY = EnGPSFrequency::freq_10_Hz;
public:

    class iterator : public std::iterator<std::forward_iterator_tag, ConfigFrame> {
        friend class Config;
    private:
       map <unsigned int, ConfigFrame*>::iterator innerIterator;
       Config* pConfig;
    public:
       iterator(map <unsigned int, ConfigFrame*>::iterator it, Config* pConfig);
       bool operator==(const Config::iterator& second) const;
       bool operator!=(const Config::iterator& second) const;
       ConfigFrame* operator*();
       ConfigFrame* operator->();
       iterator& operator++();
       iterator operator++(int);
    };

    class const_iterator : public std::iterator<std::forward_iterator_tag, const ConfigFrame> {
        friend class Config;
    private:
       map <unsigned int, ConfigFrame*>::const_iterator innerIterator;
       const Config* pConfig;
    public:
       const_iterator(map <unsigned int, ConfigFrame*>::const_iterator it, const Config* pConfig);
       bool operator==(const Config::const_iterator& second) const;
       bool operator!=(const Config::const_iterator& second) const;
       const ConfigFrame* operator*();
       const ConfigFrame* operator->();
       const_iterator& operator++();
       const_iterator operator++(int);
    };

    static int getActualVersion();
    static int getActualSubVersion();

    Config();

    void            setVersion(int sVersion);
    void            setSubVersion(int sSubVersion);
    void            setCANBitrate(EnCANBitrate bitrate);
    void            setGPSFrequency(EnGPSFrequency frequency);

    int             getVersion() const;
    int             getSubVersion() const;
    EnCANBitrate    getCANBitrate() const;
    EnGPSFrequency  getGPSFrequency() const;
    int             getNumOfFrames() const;

    ConfigFrame*    getFrameById(unsigned int id);

    iterator        begin();
    iterator        end();
    const_iterator  cbegin() const;
    const_iterator  cend() const;

    bool            hasFrameWithId(unsigned int id) const;

    void            addFrame(ConfigFrame* aFrame);
    void            removeFrameById(unsigned int id);
    void            reset();

    void            writeToBin(WritingClass& writer) override;
    void            readFromBin(ReadingClass& reader) override;

    virtual ~Config() override;
};


#endif // CONFIG_H
