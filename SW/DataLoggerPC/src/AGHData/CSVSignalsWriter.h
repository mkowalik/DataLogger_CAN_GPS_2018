#ifndef CSV_CHANNELS_WRITER_H
#define CSV_CHANNELS_WRITER_H

#include "AGHData/DataFileClass.h"
#include "AGHUtils/WritingClass.h"
#include "AGHData/CSVWriter.h"
#include <unordered_map>

using namespace std;

/*class CSVColumn {
private:
    string id;
    string firstRow;
public:
    CSVColumn (string id, string firstRow) : id(id), firstRow(firstRow){ }
    string getId() {return id;}
    string getFirstRow() {return firstRow;}
};*/



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
    virtual void writeSingleRow(unsigned int msTime, map<const ConfigFrame*, const SingleCANFrameData*>& actualCANFramesToWrite, const SingleGPSFrameData* actualGPSValue);
public:
    virtual ~CSVSignalsWriter() override;
};

#endif // CSV_WRITER_H
