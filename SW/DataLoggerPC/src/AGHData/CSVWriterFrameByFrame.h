#ifndef CSV_WRITER_FRAME_BY_FRAME_H
#define CSV_WRITER_FRAME_BY_FRAME_H

#include "CSVWriter.h"

class CSVWriterFrameByFrame : public CSVWriter
{
private:
    char            decimalSeparator;
    const Config*   pConfig;
    WritingClass&   writer;

    void            writeHeaderRow();

    void            writeTimeAndId(const SingleCANFrameData* pCanFrame);
    void            writeTimeAndId(const SingleGPSFrameData* pGpsFrame);

    void            writeCANData(const SingleCANFrameData* pCanFrame);
    void            writeGPSData(const SingleGPSFrameData* pGpsFrame);
public:
                    CSVWriterFrameByFrame(char decimalSeparator, const Config* pConfig, WritingClass& writer);

    virtual void    writeToCSV(const DataFileClass& dataFileClass) override;

    virtual         ~CSVWriterFrameByFrame() override;
};

#endif // CSV_WRITER_FRAME_BY_FRAME_H
