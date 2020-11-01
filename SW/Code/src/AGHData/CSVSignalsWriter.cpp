#include "AGHData/CSVSignalsWriter.h"

using namespace std;

CSVSignalsWriter::CSVSignalsWriter (char decimalSeparator, const Config* pConfig, WritingClass& writer)
    : decimalSeparator(decimalSeparator), pConfig(pConfig), writer(writer){

    for (auto frameIt = pConfig->cbeginFrames(); frameIt != pConfig->cendFrames(); frameIt++){
        for (auto signalIt = (*frameIt)->cbeginSignals(); signalIt != (*frameIt)->cendSignals(); signalIt++){
            columsOrderVector.push_back(*signalIt);
        }
    }
}

void CSVSignalsWriter::writeSingleRow(unsigned int msTime,
                                      const map<const ConfigFrame*, const SingleCANFrameData*>& actualCANFramesToWriteMap,
                                      const CANErrorCode canErrorCode,
                                      const SingleGPSFrameData* pActualGPSValue)
{

    writer.write_int_to_string(msTime, false);
    writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);

    for (const ConfigSignal* pSignal: this->columsOrderVector){
        if (actualCANFramesToWriteMap.find(pSignal->getParentFrame()) != actualCANFramesToWriteMap.end()){
            const SingleCANFrameData* pActualFrame = actualCANFramesToWriteMap.at(pSignal->getParentFrame());
            try {
                if (pSignal->getDivider() != 1){
                    writer.write_double_to_string(pSignal->getSymbolicValueFromFramePayload(pActualFrame->getRawData()), CSVSignalsWriter::CSV_DOUBLE_DECIMAL_FIGURES, this->decimalSeparator, false);
                } else {
                    writer.write_int_to_string(pSignal->getSymbolicIntValueFromFramePayload(pActualFrame->getRawData()), false);
                }
            } catch (ConfigSignal::SignalExceedsDataDLCException& e) {
                string warn =
                        "Signal: " +
                        pSignal->getSignalName() +
                        " definition exceeds DLC of received frame: " +
                        pSignal->getParentFrame()->getFrameName() +
                        " [" +
                        std::to_string(pSignal->getParentFrame()->getFrameID()) +
                        "]";
                if (warnings.find(warn) != warnings.end()){
                    warnings[warn]++;
                } else {
                    warnings[warn] = 1;
                }
            }
        }
        writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);
    }

    if (!canErrorCode.noError()){
        writer.write_string(errorCodeToString(canErrorCode), false);
    }
    writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);

    if (this->pConfig->getGPSFrequency() != Config::EnGPSFrequency::freq_GPS_OFF){

        if (pActualGPSValue != nullptr){
            writer.write_int_to_string(pActualGPSValue->getGpsDateTime().tm_year, false, 4);
            writer.write_string("-", false);
            writer.write_int_to_string(pActualGPSValue->getGpsDateTime().tm_mon, false, 2);
            writer.write_string("-", false);
            writer.write_int_to_string(pActualGPSValue->getGpsDateTime().tm_mday, false, 2);
        }
        writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);

        if (pActualGPSValue != nullptr){
            writer.write_int_to_string(pActualGPSValue->getGpsDateTime().tm_hour, false, 2);
            writer.write_string(":", false);
            writer.write_int_to_string(pActualGPSValue->getGpsDateTime().tm_min, false, 2);
            writer.write_string(":", false);
            writer.write_int_to_string(pActualGPSValue->getGpsDateTime().tm_sec, false, 2);
        }
        writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);

        if (pActualGPSValue != nullptr){
            writer.write_double_to_string(pActualGPSValue->getLongitude().getDoubleVal(), CSVWriter::CSV_DOUBLE_DECIMAL_FIGURES, this->decimalSeparator, false);
        }
        writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);

        if (pActualGPSValue != nullptr){
            writer.write_double_to_string(pActualGPSValue->getLatitude().getDoubleVal(), CSVWriter::CSV_DOUBLE_DECIMAL_FIGURES, this->decimalSeparator, false);
        }
        writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);

        if (pActualGPSValue != nullptr){
            writer.write_int_to_string(pActualGPSValue->getNSatellites(), false);
        }
        writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);

        if (pActualGPSValue != nullptr){
            writer.write_double_to_string(pActualGPSValue->getAltitude().getDoubleVal(), CSVWriter::CSV_DOUBLE_DECIMAL_FIGURES, this->decimalSeparator, false);
        }
        writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);

        if (pActualGPSValue != nullptr){
            writer.write_double_to_string(pActualGPSValue->getSpeed().getDoubleVal(), CSVWriter::CSV_DOUBLE_DECIMAL_FIGURES, this->decimalSeparator, false);
        }
        writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);

        if (pActualGPSValue != nullptr){
            writer.write_double_to_string(pActualGPSValue->getTrackAngle().getDoubleVal(), CSVWriter::CSV_DOUBLE_DECIMAL_FIGURES, this->decimalSeparator, false);
        }
        writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);

        if (pActualGPSValue != nullptr){
            switch(pActualGPSValue->getFixType()){
            case SingleGPSFrameData::EnGPSFixType::Fix_NoFix:
                writer.write_string("No fix", false);
                break;
            case SingleGPSFrameData::EnGPSFixType::Fix_2DFix:
                writer.write_string("2D", false);
                break;
            case SingleGPSFrameData::EnGPSFixType::Fix_3DFix:
                writer.write_string("3D", false);
                break;
            }
        }
        writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);

        if (pActualGPSValue != nullptr){
            writer.write_double_to_string(pActualGPSValue->getHorizontalPrecision().getDoubleVal(), CSVWriter::CSV_DOUBLE_DECIMAL_FIGURES, this->decimalSeparator, false);
        }
        writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);

        if (pActualGPSValue != nullptr){
            writer.write_double_to_string(pActualGPSValue->getVerticalPrecision().getDoubleVal(), CSVWriter::CSV_DOUBLE_DECIMAL_FIGURES, this->decimalSeparator, false);
        }
        writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);
    }
    writer.write_string("\r\n", false);

}

void CSVSignalsWriter::writeHeaderRow(){

    writer.write_string("time [ms]", false);
    writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);
    for (const ConfigSignal* pSignal : this->columsOrderVector){
        writer.write_string(pSignal->getSignalName() + " [" + pSignal->getUnitName() + "]", false);
        writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);
    }
    writer.write_string("CAN Bus Error", false);
    writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);
    if (this->pConfig->getGPSFrequency() != Config::EnGPSFrequency::freq_GPS_OFF){
        writer.write_string("gps date[YYYY-MM-DD]", false);
        writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);
        writer.write_string("gps time[HH:MM:SS]", false);
        writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);
        writer.write_string("longitude", false);
        writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);
        writer.write_string("latitude", false);
        writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);
        writer.write_string("satelites available", false);
        writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);
        writer.write_string("altitude [m]", false);
        writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);
        writer.write_string("speed [km/h]", false);
        writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);
        writer.write_string("track angle [deg]", false);
        writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);
        writer.write_string("fix type {No fix|2D|3D}", false);
        writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);
        writer.write_string("horizontalPrecision", false);
        writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);
        writer.write_string("verticalPrecision", false);
        writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);
    }
    writer.write_string("\r\n", false);
}

CSVSignalsWriter::~CSVSignalsWriter(){

}
