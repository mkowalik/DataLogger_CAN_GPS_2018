#ifndef CSV_WRITER_H
#define CSV_WRITER_H

#include "DataFileClass.h"

class CSVWriter {
protected:
    static constexpr char CSV_COLUMNS_SEPARATOR         = ';';
    static constexpr int  CSV_DOUBLE_DECIMAL_FIGURES    = 3;
public:
    virtual void        writeToCSV(const DataFileClass& dataFileClass) = 0;
    virtual std::string errorCodeToString(CANErrorCode errorCode);
    virtual             ~CSVWriter(){ }
};

#endif // CSV_WRITER_H
