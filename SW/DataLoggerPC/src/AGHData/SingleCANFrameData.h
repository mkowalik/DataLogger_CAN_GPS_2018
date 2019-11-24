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
    unsigned char           rawData[ConfigFrame::MAX_FRAME_BYTES_LENGTH];

    const ConfigFrame*      pConfigFrame;

public:
    SingleCANFrameData(unsigned int msTime, const ConfigFrame* frame);

    const ConfigFrame*  getFrameConfig() const;

    unsigned int        getMsTime() const;
    unsigned int        getFrameID() const;
    unsigned int        getFrameDLC() const;
    unsigned char       getRawDataValue(unsigned int byteIndex) const;

    virtual void        readFromBin(ReadingClass& reader) override;

    void                setRawData(unsigned char* data);

//    bool                equalIDAndPayload(const SingleCANFrameData& b) const;

    unsigned long long  getSignalValueRaw(const ConfigSignal* pChannel) const;
    double              getSignalValueTransformed(const ConfigSignal* pChannel) const;
    long long           getSignalValueTransformedLL(const ConfigSignal* pChannel) const;

};

#endif // SINGLE_CAN_FRAME_DATA_H
