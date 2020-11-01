#ifndef DATA_FILE_CLASS_H
#define DATA_FILE_CLASS_H

#include <fstream>
#include <map>
#include <set>
#include <ctime>

#include "AGHConfig/Config.h"

#include "AGHUtils/ReadingClass.h"
#include "AGHUtils/RawDataParser.h"
#include "AGHUtils/FixedPoint.h"
#include "AGHData/SingleCANFrameData.h"
#include "AGHData/SingleCANErrorData.h"
#include "AGHData/SingleGPSFrameData.h"
#include "AGHData/WritableToCSV.h"

using namespace std;

/*****      DataFileClass       *****/

class DataFileClass : public ReadableFromBin {
public:
    using DataRow = std::variant<const SingleCANFrameData*, const SingleCANErrorData*, const SingleGPSFrameData*>;
private:
    static constexpr unsigned int   GPS_DATA_ID                       = 0x800;
    static constexpr unsigned int   CAN_ERROR_ID                      = 0x801;
    static constexpr unsigned int   DEFAULT_CHANNEL_RAW_VALUE         = 0;
    static constexpr double         DEFAULT_CHANNEL_TRANSFORMED_VALUE = 0.0;
    Config*                         pConfig;
    tm                              startTime;

    vector<DataRow>                 dataRows;

    void freeMemory();

public:

    DataFileClass();

    const Config&                               getConfig() const;
    const tm                                    getStartTime() const;

    virtual void                                readFromBin(ReadingClass& reader) override;

    const vector<DataRow>&                      getDataRows() const {return this->dataRows;}

    virtual std::map<std::string, unsigned int> write_to_csv(WritableToCSV::FileTimingMode mode, WritingClass& writer, char decimalSeparator, bool writeOnlyChangedValues);

    ~DataFileClass() override;

};

#endif // DATA_FILE_CLASS_H
