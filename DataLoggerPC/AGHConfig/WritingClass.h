#ifndef WRITINGCLASS_H
#define WRITINGCLASS_H

#include <fstream>
#include "AGHData/raw_data_parser.h"

using namespace std;


class WritingClass {
private:
    static const unsigned int BUFFER_SIZE = 128;
    char            decimalSeparator;
    ofstream        fileStream;
    RawDataParser   dataParser;
    char            buffer[BUFFER_SIZE];
    void            clear_buffer(size_t length);
public:
    WritingClass(string nameOfFile, RawDataParser& dataParser);
    void            write_uint8(unsigned int aValue);
    void            write_uint16(unsigned int aValue, RawDataParser::EndianessMode endianessMode);
    void            write_uint32(unsigned int aValue, RawDataParser::EndianessMode endianessMode);
    void            write_int16(int aValue, RawDataParser::EndianessMode endianessMode);
    void            write_string(string aStr, bool writeTerminatingZero, int aLength = -1);

    void            write_double_to_string(double value, int decimal_figures, char decimalSeparator, bool writeTerminatingZero);
    void            write_int_to_string(int value, bool writeTerminatingZero);
};

class WritableToBin {
public:
    virtual void    write_to_bin(WritingClass& writer) = 0;
    virtual         ~WritableToBin() = 0;
};

class WritableToCSV {
public:
    enum FileTimingMode {
        EventMode = 0,
        StaticPeriod10HzMode,
        StaticPeriod100HzMode,
        StaticPeriod250HzMode,
        StaticPeriod500HzMode,
        StaticPeriod1000HzMode
    };

    virtual void    write_to_csv(FileTimingMode mode, WritingClass& writer, char decimalSeparator) = 0;
    virtual         ~WritableToCSV() = 0;
};

#endif // WRITINGCLASS_H
