#include "can_channel_with_last_value.h"

CANChannelWithLastValue::CANChannelWithLastValue(const ConfigFrame& frame_, const ConfigChannel& channel_) :
    frame(frame_), channel(channel_), lastValue(nullptr), wasRead(false) {
}

void CANChannelWithLastValue::setValue(const SingleChannelData& lastValue_){
    lastValue = &lastValue_;
    wasRead = false;
}

const ConfigChannel& CANChannelWithLastValue::getConfigChannel() const {
    return channel;
}

const ConfigFrame& CANChannelWithLastValue::getConfigFrame() const {
    return frame;
}

const SingleChannelData& CANChannelWithLastValue::getLastValue() const {
    if (lastValue == nullptr){
        throw logic_error("Reference to last value is invalid.");
    }
    wasRead = true;
    return *lastValue;
}

bool CANChannelWithLastValue::isLastValueValid() const {
    return lastValue != nullptr;
}

bool CANChannelWithLastValue::wasValueRead() const {
    return wasRead;
}
