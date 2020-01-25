#ifndef SINGLE_CAN_FRAME_DATA_H
#define SINGLE_CAN_FRAME_DATA_H

#include <vector>
#include <array>

#include "AGHConfig/ConfigFrame.h"
#include "AGHConfig/ConfigSignal.h"

using namespace std;

class SingleCANFrameData : public ReadableFromBin {
private:

    unsigned int            msTime;
    vector<unsigned char>   rawPayloadData;

    const ConfigFrame*      pConfigFrame;

public:
    SingleCANFrameData(unsigned int msTime, const ConfigFrame* pConfigFrame, ReadingClass &reader);
    SingleCANFrameData(unsigned int msTime, const ConfigFrame* pConfigFrame, vector<unsigned char> rawPayloadData);
    SingleCANFrameData(unsigned int msTime, const ConfigFrame* pConfigFrame, unsigned char* data, unsigned int dlc);

    const ConfigFrame*      getFrameConfig() const;

    unsigned int            getMsTime() const;
    unsigned int            getFrameID() const;
    unsigned int            getFrameDLC() const;
    vector<unsigned char>   getRawData() const;
    unsigned char           getRawDataByte(unsigned int byteIndex) const;

    virtual void            readFromBin(ReadingClass& reader) override;

    void                    setRawPayloadData(vector<unsigned char> data);
    void                    setRawPayloadData(unsigned char* data, unsigned int dlc);

    unsigned long long      getSignalValueRaw(const ConfigSignal* pChannel) const;

};

#endif // SINGLE_CAN_FRAME_DATA_H
