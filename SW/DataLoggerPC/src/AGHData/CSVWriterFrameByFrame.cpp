#include "CSVWriterFrameByFrame.h"

CSVWriterFrameByFrame::CSVWriterFrameByFrame(char decimalSeparator, const Config* pConfig, WritingClass& writer) :
    decimalSeparator(decimalSeparator), pConfig(pConfig), writer(writer) {
}

void CSVWriterFrameByFrame::writeHeaderRow() {

    writer.write_string("time [ms];", false);
    writer.write_string("ID;", false);
    writer.write_string("DLC;", false);
    for (int i=0; i<ConfigFrame::MAX_FRAME_BYTES_LENGTH; i++){
        writer.write_string("data[", false);
        writer.write_int_to_string(i, false);
        writer.write_string("];", false);
    }
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

void CSVWriterFrameByFrame::writeTimeAndId(const SingleCANFrameData* pCanFrame){
    if (pCanFrame == nullptr){
        throw std::invalid_argument("Null pointer in argument.");
    }

    writer.write_int_to_string(pCanFrame->getMsTime(), false);
    writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);

    writer.write_int_to_string(pCanFrame->getFrameID(), false);
    writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);
}

void CSVWriterFrameByFrame::writeTimeAndId(const SingleGPSFrameData* pGpsFrame){
    if (pGpsFrame == nullptr){
        throw std::invalid_argument("Null pointer in argument.");
    }

    writer.write_int_to_string(pGpsFrame->getMsTime(), false);
    writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);

    writer.write_string("GPS", false);
    writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);
}

void CSVWriterFrameByFrame::writeCANData(const SingleCANFrameData* pCanFrame){

    if (pCanFrame != nullptr){
        writer.write_int_to_string(pCanFrame->getFrameDLC(), false);
    }
    writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);

    if (pCanFrame != nullptr){
        for (unsigned int i = 0; i < pCanFrame->getFrameDLC(); i++){
            writer.write_int_to_string(static_cast<unsigned int>(pCanFrame->getRawDataValue(i)), false);
            writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);
        }
    }
    for (unsigned int i = (pCanFrame!=nullptr) ? pCanFrame->getFrameDLC() : 0; i < ConfigFrame::MAX_FRAME_BYTES_LENGTH; i++){
        writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);
    }
}

void CSVWriterFrameByFrame::writeGPSData(const SingleGPSFrameData* actualGPSValue){

    if (this->pConfig->getGPSFrequency() == Config::EnGPSFrequency::freq_GPS_OFF){
        return;
    }

    if (actualGPSValue != nullptr){
        writer.write_int_to_string(actualGPSValue->getGpsDateTime().tm_year, false, 4);
        writer.write_string("-", false);
        writer.write_int_to_string(actualGPSValue->getGpsDateTime().tm_mon, false, 2);
        writer.write_string("-", false);
        writer.write_int_to_string(actualGPSValue->getGpsDateTime().tm_mday, false, 2);
    }
    writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);

    if (actualGPSValue != nullptr){
        writer.write_int_to_string(actualGPSValue->getGpsDateTime().tm_hour, false, 2);
        writer.write_string(":", false);
        writer.write_int_to_string(actualGPSValue->getGpsDateTime().tm_min, false, 2);
        writer.write_string(":", false);
        writer.write_int_to_string(actualGPSValue->getGpsDateTime().tm_sec, false, 2);
    }
    writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);

    if (actualGPSValue != nullptr){
        writer.write_double_to_string(actualGPSValue->getLongitude().getDoubleVal(), CSVWriter::CSV_DOUBLE_DECIMAL_FIGURES, this->decimalSeparator, false);
    }
    writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);

    if (actualGPSValue != nullptr){
        writer.write_double_to_string(actualGPSValue->getLatitude().getDoubleVal(), CSVWriter::CSV_DOUBLE_DECIMAL_FIGURES, this->decimalSeparator, false);
    }
    writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);

    if (actualGPSValue != nullptr){
        writer.write_int_to_string(actualGPSValue->getNSatellites(), false);
    }
    writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);

    if (actualGPSValue != nullptr){
        writer.write_double_to_string(actualGPSValue->getAltitude().getDoubleVal(), CSVWriter::CSV_DOUBLE_DECIMAL_FIGURES, this->decimalSeparator, false);
    }
    writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);

    if (actualGPSValue != nullptr){
        writer.write_double_to_string(actualGPSValue->getSpeed().getDoubleVal(), CSVWriter::CSV_DOUBLE_DECIMAL_FIGURES, this->decimalSeparator, false);
    }
    writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);

    if (actualGPSValue != nullptr){
        writer.write_double_to_string(actualGPSValue->getTrackAngle().getDoubleVal(), CSVWriter::CSV_DOUBLE_DECIMAL_FIGURES, this->decimalSeparator, false);
    }
    writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);

    if (actualGPSValue != nullptr){
        switch(actualGPSValue->getFixType()){
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

    if (actualGPSValue != nullptr){
        writer.write_double_to_string(actualGPSValue->getHorizontalPrecision().getDoubleVal(), CSVWriter::CSV_DOUBLE_DECIMAL_FIGURES, this->decimalSeparator, false);
    }
    writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);

    if (actualGPSValue != nullptr){
        writer.write_double_to_string(actualGPSValue->getVerticalPrecision().getDoubleVal(), CSVWriter::CSV_DOUBLE_DECIMAL_FIGURES, this->decimalSeparator, false);
    }
    writer.write_char(CSVWriter::CSV_COLUMNS_SEPARATOR);
}

void CSVWriterFrameByFrame::writeToCSV(const DataFileClass &dataFileClass){

    this->writeHeaderRow();

    vector<SingleCANFrameData*>::const_iterator canIt = dataFileClass.getCANData().cbegin();
    vector<SingleGPSFrameData*>::const_iterator gpsIt = dataFileClass.getGPSData().cbegin();

    while (true){

        if ((canIt == dataFileClass.getCANData().cend()) && (gpsIt == dataFileClass.getGPSData().cend())) {
            break;
        }

        if ((canIt != dataFileClass.getCANData().cend() && gpsIt != dataFileClass.getGPSData().cend() && ((*canIt)->getMsTime() <= (*gpsIt)->getMsTime())) ||
            (canIt != dataFileClass.getCANData().cend() && gpsIt == dataFileClass.getGPSData().cend())) {

            this->writeTimeAndId(*canIt);
            this->writeCANData(*canIt);
            this->writeGPSData(nullptr);

            writer.write_string("\r\n", false);
            ++canIt;
            continue;
        }

        if ((canIt != dataFileClass.getCANData().cend() && gpsIt != dataFileClass.getGPSData().cend() && ((*gpsIt)->getMsTime() <= (*canIt)->getMsTime())) ||
            (canIt == dataFileClass.getCANData().cend() && gpsIt != dataFileClass.getGPSData().cend())) {

            this->writeTimeAndId(*gpsIt);
            this->writeCANData(nullptr);
            this->writeGPSData(*gpsIt);

            writer.write_string("\r\n", false);
            ++gpsIt;
            continue;
        }
    }
}

CSVWriterFrameByFrame::~CSVWriterFrameByFrame(){

}
