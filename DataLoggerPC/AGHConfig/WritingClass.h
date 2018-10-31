#ifndef WRITINGCLASS_H
#define WRITINGCLASS_H

#include <fstream>

using namespace std;


class WritingClass {
private:
    static const unsigned int BUFFER_SIZE = 128;
    ofstream    fileStream;
    char        buffer[BUFFER_SIZE];
    void        clear_buffer(size_t length);
    void        write_little_endian(unsigned int aValue, unsigned int number_of_bytes);
public:
    WritingClass(string nameOfFile);
    void        write_uint8(unsigned int aValue);
    void        write_uint16(unsigned int aValue);
    void        write_uint32(unsigned int aValue);
    void        write_int16(int aValue);

    void        write_string(string aStr, int aLength = -1);
};

class WritableToBin {
public:
    virtual void write_to_bin(WritingClass& writer) = 0;
    virtual ~WritableToBin() = 0;
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

    virtual void write_to_csv(FileTimingMode mode, WritingClass& writer) = 0;
    virtual ~WritableToCSV() = 0;
};

#endif // WRITINGCLASS_H
