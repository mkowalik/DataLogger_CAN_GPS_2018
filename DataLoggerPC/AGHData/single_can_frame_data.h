#ifndef SINGLE_CAN_FRAME_DATA_H
#define SINGLE_CAN_FRAME_DATA_H

#include <vector>
#include <array>

#include "AGHConfig/ConfigFrame.h"
#include "AGHConfig/ConfigChannel.h"

using namespace std;

/*****      SingleChannelData       *****/

class SingleChannelData {
    const ConfigChannel&    channel;
    int                     value;
public:
    SingleChannelData(const ConfigChannel& channel, int value);

    const ConfigChannel&    get_channel() const;
    int                     get_value_raw() const;
    double                  get_value_transformed() const;
    int                     get_value_transformed_int() const;

    operator double() const;
    operator int() const;
};

class SingleCANFrameData : public ReadableFromBin {
private:

    unsigned int                msTime;
    vector<SingleChannelData>   channelsData;
    unsigned char               rawData[ConfigFrame::MAX_FRAME_BYTES_LENGTH];


    const ConfigFrame&          frameConfig;
    const RawDataParser&        dataParser;

public:
    SingleCANFrameData(unsigned int msTime, const ConfigFrame& frame, const RawDataParser& dataParser);

    unsigned int                        getMsTime() const;
    const vector<SingleChannelData>&    getData() const;
    int                                 getFrameID() const;
    unsigned int                        getFrameDLC() const;
    unsigned char                       getRawDataValue(unsigned int byteIndex) const;

    void                                read_from_bin(ReadingClass& reader) override;

};

#endif // SINGLE_CAN_FRAME_DATA_H
