#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <map>

#include "AGHUtils/ReadingClass.h"
#include "AGHUtils/WritingClass.h"

#include "AGHConfig/ConfigFrame.h"
#include "AGHConfig/ConfigSignal.h"
#include "AGHConfig/ConfigTrigger.h"

class Config : public WritableToBin, public ReadableFromBin {
    friend class ConfigFrame;
public:
    enum class EnCANBitrate {
        bitrate_50kbps  = 50,
        bitrate_125kbps = 125,
        bitrate_250kbps = 250,
        bitrate_500kbps = 500,
        bitrate_1Mbps   = 1000,
    };
    enum class EnGPSFrequency {
        freq_GPS_OFF = 0,
        freq_0_5_Hz  = 1,
        freq_1_Hz    = 2,
        freq_2_Hz    = 3,
        freq_5_Hz    = 4,
        freq_10_Hz   = 5
    };
    static constexpr unsigned int   ACTUAL_VERSION          = 0;
    static constexpr unsigned int   ACTUAL_SUB_VERSION      = 4;

    using FramesIterator        = vector<ConfigFrame*>::iterator;
    using ConstFramesIterator   = vector<ConfigFrame*>::const_iterator;

    using TriggersIterator      = vector<ConfigTrigger*>::iterator;
    using ConstTriggersIterator = vector<ConfigTrigger*>::const_iterator;
private:
    static constexpr EnCANBitrate   DEFAULT_CAN_BITRATE     = EnCANBitrate::bitrate_500kbps;
    static constexpr EnGPSFrequency DEFAULT_GPS_FREQUENCY   = EnGPSFrequency::freq_10_Hz;
    static constexpr unsigned int   DEFAULT_RTC_CONFIGURATION_FRAME_ID = 0x7FF;

    static constexpr int            START_CONFIG_TRIGGERS_MAX_NUMBER    = 5;
    static constexpr int            STOP_CONFIG_TRIGGERS_MAX_NUMBER     = 5;

    static constexpr unsigned int   CONFIG_NAME_LENGTH = 20;

    Config();

    void                                        addFrame(ConfigFrame *pFrame);
    std::vector<ConfigFrame*>::const_iterator   lowerBoundFrameConstIterator(unsigned int frameID) const;
    std::vector<ConfigFrame*>::iterator         lowerBoundFrameIterator(unsigned int frameID);
    bool                                        framesEmpty() const;
    void                                        sortFramesCallback();

    unsigned int                version;
    unsigned int                subVersion;
    std::string                 logFileName;
    EnCANBitrate                canBitrate;
    EnGPSFrequency              gpsFrequency;
    unsigned int                rtcConfigurationFrameID;

    std::vector <ConfigFrame*>  framesVector;

    std::vector<ConfigTrigger*> startConfigTriggers;
    std::vector<ConfigTrigger*> stopConfigTriggers;

public:

    Config(std::string logFileName, EnCANBitrate canBitrate, EnGPSFrequency gpsFrequency, unsigned int rtcConfigurationFrameID);
    Config(unsigned int version, unsigned int subVersion, std::string logFileName, EnCANBitrate canBitrate, EnGPSFrequency gpsFrequency, unsigned int rtcConfigurationFrameID);
    Config(ReadingClass& reader);

    //<----- Access to preambule data ----->/
    void                    setVersion(unsigned int sVersion);
    void                    setSubVersion(unsigned int sSubVersion);
    void                    setLogFileName(const std::string logFileName);
    void                    setCANBitrate(EnCANBitrate bitrate);
    void                    setGPSFrequency(EnGPSFrequency frequency);
    void                    setRTCConfigurationFrameID(unsigned int frameID);

    unsigned int            getVersion() const;
    unsigned int            getSubVersion() const;
    std::string             getLogFileName() const;
    EnCANBitrate            getCANBitrate() const;
    EnGPSFrequency          getGPSFrequency() const;
    unsigned int            getRTCConfigurationFrameID() const;

    //<----- Access to frames definitions ----->/

    int                     getNumOfFrames() const;

    bool                    hasFrameWithId(unsigned int frameID) const;
    ConfigFrame*            getFrameWithId(unsigned int frameID) const;
    ConfigFrame*            addFrame(unsigned int frameID, string frameName);
    void                    removeFrame(const FramesIterator frameIterator);
    void                    removeFrame(unsigned int frameID);

    bool                    hasSignal(unsigned int frameID, unsigned int signalID) const;
    ConfigSignal*           getSignal(unsigned int frameID, unsigned int signalID) const;

    FramesIterator          beginFrames();
    FramesIterator          endFrames();
    ConstFramesIterator     cbeginFrames() const;
    ConstFramesIterator     cendFrames() const;

    //<----- Access to triggers definitions ----->/
    unsigned int            getNumberOfStartTriggers();
    unsigned int            getNumberOfStopTriggers();

    void                    addStartTrigger(ConfigTrigger* pTrigger);
    void                    removeStartTrigger(TriggersIterator* pTrigger);

    void                    addStopTrigger(ConfigTrigger* pTrigger);
    void                    removeStopTrigger(TriggersIterator* pTrigger);

    TriggersIterator        beginStartTriggers();
    TriggersIterator        endStartTriggers();
    ConstTriggersIterator   cbeginStartTriggers() const;
    ConstTriggersIterator   cendStartTriggers() const;

    TriggersIterator        beginStopTriggers();
    TriggersIterator        endStopTriggers();
    ConstTriggersIterator   cbeginStopTriggers() const;
    ConstTriggersIterator   cendStopTriggers() const;

    //<----- General purpose methods definitions ----->/
    void                    reset();

    virtual void            writeToBin(WritingClass& writer) override;
    virtual void            readFromBin(ReadingClass& reader) override;

    virtual ~Config() override;
};


#endif // CONFIG_H
