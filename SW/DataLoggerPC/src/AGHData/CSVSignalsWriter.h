#ifndef CSV_CHANNELS_WRITER_H
#define CSV_CHANNELS_WRITER_H

#include "AGHData/DataFileClass.h"
#include "AGHUtils/WritingClass.h"
#include "AGHData/CSVWriter.h"
#include <unordered_map>

using namespace std;

class CSVSignalsWriter : public CSVWriter
{
private:
    char                        decimalSeparator;
    const Config*               pConfig;
    WritingClass&               writer;
    vector<const ConfigSignal*> columsOrderVector;
protected:
                 CSVSignalsWriter (char decimalSeparator, const Config* pConfig, WritingClass& writer);
    virtual void writeHeaderRow();
    virtual void writeSingleRow(unsigned int msTime,
                                const map<const ConfigFrame*, const SingleCANFrameData*>& actualCANFramesToWriteMap,
                                const CANErrorCode canErrorCode,
                                const SingleGPSFrameData* pActualGPSValue);
public:
    virtual ~CSVSignalsWriter() override;
};

#endif // CSV_WRITER_H
