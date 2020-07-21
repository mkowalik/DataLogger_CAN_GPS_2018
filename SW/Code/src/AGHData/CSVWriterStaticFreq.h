#ifndef CSV_WRITER_STATIC_FREQ_H
#define CSV_WRITER_STATIC_FREQ_H

#include "AGHData/CSVSignalsWriter.h"


class CSVWriterStaticFreq : public CSVSignalsWriter
{
private:
    bool            writeValuesForChangedSignalsOnly;
    unsigned int    periodMs;
public:
    CSVWriterStaticFreq(char decimalSeparator, bool writeValuesForChangedSignalsOnly, const Config* pConfig, WritingClass& writer, unsigned int periodMs);
    virtual void writeToCSV(const DataFileClass& dataFileClass) override;

    virtual ~CSVWriterStaticFreq() override;
};

#endif // CSV_WRITER_STATIC_FREQ_H
