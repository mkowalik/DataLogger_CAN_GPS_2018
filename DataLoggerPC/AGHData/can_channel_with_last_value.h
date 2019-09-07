#ifndef CAN_CHANNEL_WITH_LAST_VALUE_H
#define CAN_CHANNEL_WITH_LAST_VALUE_H

#include "AGHConfig/ConfigFrame.h"
#include "AGHConfig/ConfigChannel.h"
#include "AGHData/single_can_frame_data.h"

class CANChannelWithLastValue {
private:
    const ConfigFrame&          frame;
    const ConfigChannel&        channel;
    const SingleChannelData*    lastValue;
    mutable bool                        wasRead;
public:

    CANChannelWithLastValue(const ConfigFrame& frame_, const ConfigChannel& channel_);
    void setValue(const SingleChannelData& lastValue_);
    const ConfigChannel& getConfigChannel() const;
    const ConfigFrame& getConfigFrame() const;
    const SingleChannelData& getLastValue() const;
    bool isLastValueValid() const;
    bool wasValueRead() const;
};


#endif // CAN_CHANNEL_WITH_LAST_VALUE_H
