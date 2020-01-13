#ifndef DATA_FILE_CLASS_H
#define DATA_FILE_CLASS_H

#include <fstream>
#include <map>
#include <set>
#include <ctime>

#include "AGHUtils/ReadingClass.h"
#include "AGHConfig/Config.h"
#include "AGHUtils/RawDataParser.h"
#include "AGHUtils/FixedPoint.h"
#include "AGHData/SingleCANFrameData.h"
#include "AGHData/SingleGPSFrameData.h"
#include "AGHData/WritableToCSV.h"

using namespace std;

/*****      DataFileClass       *****/

class DataFileClass : public ReadableFromBin {
private:
    static constexpr int        GPS_DATA_ID = 0x800;
    static constexpr int        DEFAULT_CHANNEL_RAW_VALUE = 0;
    static constexpr double     DEFAULT_CHANNEL_TRANSFORMED_VALUE = 0.0;
    Config*                     pConfig;
    tm                          startTime;
    vector<SingleCANFrameData*> pCanDataVector;
    vector<SingleGPSFrameData*> pGpsDataVector;

    void freeMemory();

public:

    DataFileClass();

    const Config&                       getConfig() const;
    const tm                            getStartTime() const;

    virtual void                        readFromBin(ReadingClass& reader) override;

    const vector<SingleCANFrameData*>&  getCANData() const {return this->pCanDataVector;}
    const vector<SingleGPSFrameData*>&  getGPSData() const {return this->pGpsDataVector;}

    virtual void                        write_to_csv(WritableToCSV::FileTimingMode mode, WritingClass& writer, char decimalSeparator, bool writeOnlyChangedValues);

    ~DataFileClass();

};

#endif // DATA_FILE_CLASS_H
