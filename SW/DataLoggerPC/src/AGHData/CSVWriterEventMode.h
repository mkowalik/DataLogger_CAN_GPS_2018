#ifndef CSV_WRITER_EVENT_MODE_H
#define CSV_WRITER_EVENT_MODE_H

#include "AGHData/CSVSignalsWriter.h"
#include "AGHData/DataFileClass.h"
#include "AGHUtils/WritingClass.h"

using namespace std;

class CSVWriterEventMode : public CSVSignalsWriter
{
private:
    bool    writeValuesForChangedSignalsOnly;
public:
                 CSVWriterEventMode(char decimalSeparator, bool writeValuesForChangedSignalsOnly, const Config* pConfig, WritingClass& writer);

    virtual void writeToCSV(const DataFileClass& dataFileClass) override;

    virtual      ~CSVWriterEventMode() override;
};

#endif // CSV_WRITER_EVENT_MODE_H

/*

void DataFileClass::write_single_gps_row(, const SingleGPSFrameData* pGpsData){
    if (pGpsData != nullptr){
        writer.write_int_to_string(pGpsData->getGpsDateTime().tm_year, false, 4);
        writer.write_string("-", false);
        writer.write_int_to_string(pGpsData->getGpsDateTime().tm_year, false, 2);
        writer.write_string("-", false);
        writer.write_int_to_string(pGpsData->getGpsDateTime().tm_year, false, 2);
        writer.write_string(";", false);

        writer.write_int_to_string(pGpsData->getGpsDateTime().tm_hour, false, 2);
        writer.write_string(":", false);
        writer.write_int_to_string(pGpsData->getGpsDateTime().tm_min, false, 2);
        writer.write_string(":", false);
        writer.write_int_to_string(pGpsData->getGpsDateTime().tm_sec, false, 2);
        writer.write_string(";", false);

        writer.write_double_to_string(pGpsData->getLongitude().getDoubleVal(), 3, decimalSeparator, false);
        writer.write_string(";", false);

        writer.write_double_to_string(pGpsData->getLatitude().getDoubleVal(), 3, decimalSeparator, false);
        writer.write_string(";", false);

        writer.write_int_to_string(pGpsData->getNSatellites(), false);
        writer.write_string(";", false);

        writer.write_double_to_string(pGpsData->getAltitude().getDoubleVal(), 3, decimalSeparator, false);
        writer.write_string(";", false);

        writer.write_double_to_string(pGpsData->getSpeed().getDoubleVal(), 3, decimalSeparator, false);
        writer.write_string(";", false);

        writer.write_double_to_string(pGpsData->getTrackAngle().getDoubleVal(), 3, decimalSeparator, false);
        writer.write_string(";", false);

        switch(pGpsData->getFixType()){
        case SingleGPSFrameData::EnGPSFixType::Fix_NoFix:
            writer.write_string("No fix;", false);
            break;
        case SingleGPSFrameData::EnGPSFixType::Fix_2DFix:
            writer.write_string("2D;", false);
            break;
        case SingleGPSFrameData::EnGPSFixType::Fix_3DFix:
            writer.write_string("3D;", false);
            break;
        }

        writer.write_double_to_string(pGpsData->getHorizontalPrecision().getDoubleVal(), 3, decimalSeparator, false);
        writer.write_string(";", false);

        writer.write_double_to_string(pGpsData->getVerticalPrecision().getDoubleVal(), 3, decimalSeparator, false);
        writer.write_string(";", false);

    } else {
        for (int i=0; i<GPS_DATA_FIELDS_NUMBER; i++){
            writer.write_string(";", false);
        }
    }
}

void DataFileClass::write_single_csv_data_row(unsigned int msTime,
                                              WritingClass& writer,
                                              const SingleGPSFrameData* pGpsData,
                                              vector<map<int, vector <CANChannelWithLastValue>>::iterator>& csv_frames_columns_order,
                                              bool writeOnlyChangedValues) {

    writer.write_string(to_string(msTime), false);

    for (map<int, vector <CANChannelWithLastValue>>::iterator& mapIt : csv_frames_columns_order){
        for (CANChannelWithLastValue& chanWithVal : mapIt->second){
            writer.write_string(";", false);
            if (chanWithVal.isLastValueValid()){
                if (writeOnlyChangedValues && !chanWithVal.wasValueRead()){
                    if (chanWithVal.getConfigChannel().get_divider() != 1){
                        writer.write_double_to_string(chanWithVal.getLastValue().get_value_transformed(), 3, decimalSeparator, false);
                    } else {
                        writer.write_int_to_string(chanWithVal.getLastValue().get_value_transformed_int(), decimalSeparator, false);
                    }
                } else if (!writeOnlyChangedValues){
                    if (chanWithVal.getConfigChannel().get_divider() != 1){
                        writer.write_double_to_string(chanWithVal.getLastValue().get_value_transformed(), 3, decimalSeparator, false);
                    } else {
                        writer.write_int_to_string(chanWithVal.getLastValue().get_value_transformed_int(), decimalSeparator, false);
                    }
                }
            }
        }
    }

    if (this->config.get_GPSFrequency() != Config::EnGPSFrequency::freq_GPS_OFF){
        write_single_gps_row(writer, pGpsData);
        if (writeOnlyChangedValues){
            pGpsData = nullptr;
        }
    }

    writer.write_string("\r\n", false);
}

void DataFileClass::write_to_csv_static_period_mode(WritingClass& writer,
                                                    map<int, vector <CANChannelWithLastValue>>& lastValues,
                                                    const SingleGPSFrameData* pGpsData,
                                                    vector<map<int, vector <CANChannelWithLastValue>>::iterator>& csv_frames_columns_order,
                                                    bool writeOnlyChangedValues,
                                                    unsigned int periodMs) {

    if (periodMs == 0){
        throw invalid_argument("period may not be equal to 0.");
    }


    vector<SingleCANFrameData>::const_iterator canIt = canData.cbegin();
    vector<SingleGPSFrameData>::const_iterator gpsIt = gpsData.cbegin();

    vector<SingleCANFrameData>::const_iterator nextCanIt = canData.cbegin();
    vector<SingleGPSFrameData>::const_iterator nextGpsIt = gpsData.cbegin();

    unsigned int actualMsTime = periodMs;
    unsigned int lastReadMsTime = 0;

    while (true){
        if (canIt == canData.cend() && gpsIt == gpsData.cend()){
            break;
        }

        if (canIt != canData.cend() && canIt->getMsTime() <= actualMsTime) {

            vector <CANChannelWithLastValue>::iterator lastValIt = lastValues.at(canIt->getFrameID()).begin();
            for (auto& singleChannelData: canIt->getData()){
                lastValIt->setValue(singleChannelData);
                lastValIt++;
            }
            lastReadMsTime = canIt->getMsTime();
            nextCanIt = canIt+1;
        }

        if (gpsIt != gpsData.cend() && gpsIt->getMsTime() <= actualMsTime) {
            pGpsData = &(*gpsIt);
            lastReadMsTime = gpsIt->getMsTime();
            nextGpsIt = gpsIt+1;
        }

        if (lastReadMsTime == 0){
            write_single_csv_data_row(actualMsTime,
                                      writer,
                                      pGpsData,
                                      csv_frames_columns_order,
                                      writeOnlyChangedValues);
            actualMsTime += periodMs;
        }
        canIt = nextCanIt;
        gpsIt = nextGpsIt;
        lastReadMsTime = 0;
    }
}

void DataFileClass::write_to_csv_event_mode(WritingClass& writer,
                                            map<int, vector <CANChannelWithLastValue>>& lastValues,
                                            const SingleGPSFrameData* pGpsData,
                                            vector<map<int, vector <CANChannelWithLastValue>>::iterator>& csv_frames_columns_order,
                                            bool writeOnlyChangedValues) {


}

void DataFileClass::write_to_csv_frame_by_frame_mode(WritingClass& writer) {

    vector<SingleCANFrameData>::const_iterator canIt = canData.cbegin();
    vector<SingleGPSFrameData>::const_iterator gpsIt = gpsData.cbegin();

    while (true){
        if (canIt == canData.cend() && gpsIt == gpsData.cend()){
            break;
        }

        if ((canIt != canData.cend() && gpsIt != gpsData.cend() && (canIt->getMsTime() <= gpsIt->getMsTime())) ||
            (canIt != canData.cend() && gpsIt == gpsData.cend())) {

            writer.write_int_to_string(canIt->getMsTime(), false);
            writer.write_string(";", false);

            writer.write_int_to_string(canIt->getFrameID(), false);
            writer.write_string(";", false);

            writer.write_int_to_string(canIt->getFrameDLC(), false);
            writer.write_string(";", false);

            for (unsigned int i=0; i<canIt->getFrameDLC(); i++){
                writer.write_int_to_string(static_cast<unsigned int>(canIt->getRawDataValue(i)), false);
                writer.write_string(";", false);
            }

            writer.write_string("\n", false);
            ++canIt;
        }

        if ((canIt != canData.cend() && gpsIt != gpsData.cend() && (gpsIt->getMsTime() <= canIt->getMsTime())) ||
            (canIt == canData.cend() && gpsIt != gpsData.cend())) {

            writer.write_int_to_string(gpsIt->getMsTime(), false);
            writer.write_string(";", false);

            writer.write_string("GPS;", false);

            write_single_gps_row(writer, &(*gpsIt));

            writer.write_string("\n", false);
            ++gpsIt;
        }
    }
}





void DataFileClass::write_to_csv(WritableToCSV::FileTimingMode mode, WritingClass& writer, char decimalSeparator_, bool writeOnlyChangedValues) {

    // *************  Writing down colums with time, CAN channels and GPS data   *************

    decimalSeparator = decimalSeparator_;
    map<int, vector <CANChannelWithLastValue>>                  lastValues;
    SingleGPSFrameData*                                         pGpsData = nullptr;
    vector<map<int, vector <CANChannelWithLastValue>>::iterator>  csv_frames_columns_order;

    for(Config::const_iterator frIt = config.cbegin(); frIt != config.cend(); ++frIt){
        vector <CANChannelWithLastValue> channels;
        for (auto chansIt = frIt->cbegin(); chansIt != frIt->cend(); chansIt++){
            channels.push_back(CANChannelWithLastValue(*frIt, *chansIt));
        }
        lastValues.emplace(frIt->get_ID(), channels);
    }

    if (mode == WritableToCSV::FileTimingMode::FrameByFrameMode){

        writer.write_string("time [ms];", false);
        writer.write_string("ID;", false);
        writer.write_string("DLC;", false);
        for (int i=0; i<ConfigFrame::MAX_FRAME_BYTES_LENGTH; i++){
            writer.write_string("data[", false);
            writer.write_int_to_string(i, false);
            writer.write_string("];", false);
        }

        writer.write_string("\r\n", false);

        if (config.get_GPSFrequency() != Config::EnGPSFrequency::freq_GPS_OFF){
            writer.write_string("time [ms];", false);
            writer.write_string("gps date[YYYY-MM-DD];", false);
            writer.write_string("gps time[HH:MM:SS];", false);
            writer.write_string("longitude;", false);
            writer.write_string("latitude;", false);
            writer.write_string("satelites available;", false);
            writer.write_string("altitude [m];", false);
            writer.write_string("speed [km/h];", false);
            writer.write_string("track angle [deg];", false);
            writer.write_string("fix type {No fix|2D|3D};", false);
            writer.write_string("horizontalPrecision;", false);
            writer.write_string("verticalPrecision;", false);
        }

        writer.write_string("\r\n", false);

    } else {

        writer.write_string("time [ms];", false);

        for(auto frIt = config.cbegin(); frIt != config.cend(); ++frIt){
            csv_frames_columns_order.push_back(lastValues.find(frIt->get_ID()));
            for (ConfigFrame::const_iterator chIt = frIt->cbegin(); chIt != frIt->cend(); ++chIt){
              writer.write_string(chIt->get_channelName() + " [" + chIt->get_unitName() + "]", false);
              writer.write_string(";", false);
            }
        }

        if (config.get_GPSFrequency() != Config::EnGPSFrequency::freq_GPS_OFF){
            writer.write_string("gps date[YYYY-MM-DD];", false);
            writer.write_string("gps time[HH:MM:SS];", false);
            writer.write_string("longitude;", false);
            writer.write_string("latitude;", false);
            writer.write_string("satelites available;", false);
            writer.write_string("altitude [m];", false);
            writer.write_string("speed [km/h];", false);
            writer.write_string("track angle [deg];", false);
            writer.write_string("fix type {No fix|2D|3D};", false);
            writer.write_string("horizontalPrecision;", false);
            writer.write_string("verticalPrecision;", false);
        }

        writer.write_string("\r\n", false);

    }

    // *************  Writing down data rows   *************

    switch(mode){
    case WritableToCSV::FileTimingMode::EventMode:
        write_to_csv_event_mode(writer, lastValues, pGpsData, csv_frames_columns_order, writeOnlyChangedValues);
        break;
    case WritableToCSV::FileTimingMode::FrameByFrameMode:
        write_to_csv_frame_by_frame_mode(writer);
        break;
    case WritableToCSV::FileTimingMode::StaticPeriod10HzMode:
        write_to_csv_static_period_mode(writer, lastValues, pGpsData, csv_frames_columns_order, writeOnlyChangedValues, 100);
        break;
    case WritableToCSV::FileTimingMode::StaticPeriod100HzMode:
        write_to_csv_static_period_mode(writer, lastValues, pGpsData, csv_frames_columns_order, writeOnlyChangedValues, 10);
        break;
    case WritableToCSV::FileTimingMode::StaticPeriod250HzMode:
        write_to_csv_static_period_mode(writer, lastValues, pGpsData, csv_frames_columns_order, writeOnlyChangedValues, 4);
        break;
    case WritableToCSV::FileTimingMode::StaticPeriod500HzMode:
        write_to_csv_static_period_mode(writer, lastValues, pGpsData, csv_frames_columns_order, writeOnlyChangedValues, 2);
        break;
    case WritableToCSV::FileTimingMode::StaticPeriod1000HzMode:
        write_to_csv_static_period_mode(writer, lastValues, pGpsData, csv_frames_columns_order, writeOnlyChangedValues, 1);
        break;
    }

}
*/
