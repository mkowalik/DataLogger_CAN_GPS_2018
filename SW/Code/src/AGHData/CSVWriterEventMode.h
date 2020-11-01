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
