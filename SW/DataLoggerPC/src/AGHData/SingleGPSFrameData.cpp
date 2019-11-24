#include "SingleGPSFrameData.h"

using namespace std;

SingleGPSFrameData::SingleGPSFrameData(unsigned int msTime, const RawDataParser& dataParser) :
    msTime(msTime),
    longitude(GPS_FIXED_POINT_FRACTIONAL_BITS),
    latitude(GPS_FIXED_POINT_FRACTIONAL_BITS),
    altitude(GPS_FIXED_POINT_FRACTIONAL_BITS),
    speed(GPS_FIXED_POINT_FRACTIONAL_BITS),
    trackAngle(GPS_FIXED_POINT_FRACTIONAL_BITS),
    horizontalPrecision(GPS_FIXED_POINT_FRACTIONAL_BITS),
    verticalPrecision(GPS_FIXED_POINT_FRACTIONAL_BITS),
    dataParser(dataParser) {
}


unsigned int SingleGPSFrameData::getMsTime() const {
    return msTime;
}

tm SingleGPSFrameData::getGpsDateTime() const {
    return gpsDateTime;
}

FixedPoint SingleGPSFrameData::getLongitude() const {
    return longitude;
}

FixedPoint SingleGPSFrameData::getLatitude() const {
    return latitude;
}

int SingleGPSFrameData::getNSatellites() const {
    return nSatellites;
}

FixedPoint SingleGPSFrameData::getAltitude() const {
    return altitude;
}

FixedPoint SingleGPSFrameData::getSpeed() const {
    return speed;
}

FixedPoint SingleGPSFrameData::getTrackAngle() const {
    return trackAngle;
}

SingleGPSFrameData::EnGPSFixType SingleGPSFrameData::getFixType() const {
    return fixType;
}

FixedPoint SingleGPSFrameData::getHorizontalPrecision() const {
    return horizontalPrecision;
}

FixedPoint SingleGPSFrameData::getVerticalPrecision() const {
    return verticalPrecision;
}

void SingleGPSFrameData::readFromBin(ReadingClass &reader){

    gpsDateTime.tm_year = static_cast<int>(reader.reading_uint16(RawDataParser::UseDefaultEndian));
    gpsDateTime.tm_mon  = static_cast<int>(reader.reading_uint8());
    gpsDateTime.tm_mday = static_cast<int>(reader.reading_uint8());
    gpsDateTime.tm_hour = static_cast<int>(reader.reading_uint8());
    gpsDateTime.tm_min  = static_cast<int>(reader.reading_uint8());
    gpsDateTime.tm_sec  = static_cast<int>(reader.reading_uint8());

    longitude.setRawIntegerValue(static_cast<int>(reader.reading_uint32(RawDataParser::UseDefaultEndian)));
    latitude.setRawIntegerValue(static_cast<int>(reader.reading_uint32(RawDataParser::UseDefaultEndian)));
    nSatellites = static_cast<int>(reader.reading_uint8());
    altitude.setRawIntegerValue(static_cast<int>(reader.reading_uint32(RawDataParser::UseDefaultEndian)));
    speed.setRawIntegerValue(static_cast<int>(reader.reading_uint32(RawDataParser::UseDefaultEndian)));
    trackAngle.setRawIntegerValue(static_cast<int>(reader.reading_uint32(RawDataParser::UseDefaultEndian)));
    fixType = static_cast<SingleGPSFrameData::EnGPSFixType>(reader.reading_uint8());
    horizontalPrecision.setRawIntegerValue(static_cast<int>(reader.reading_uint32(RawDataParser::UseDefaultEndian)));
    verticalPrecision.setRawIntegerValue(static_cast<int>(reader.reading_uint32(RawDataParser::UseDefaultEndian)));
}
