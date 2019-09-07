#ifndef READINGCLASS_H
#define READINGCLASS_H
#include <fstream>
#include <string>
#include "AGHData/raw_data_parser.h"

using namespace std;

class ReadingClass{
private:
    static const int            BUFFER_SIZE = 128;
    RawDataParser&              dataParser;
    unsigned char               buffer[BUFFER_SIZE];
    ifstream                    data;
    streamoff                   fileSize;

    void                        clear_buffer(size_t length);
public:
    ReadingClass(string nameOfFile, RawDataParser& dataParser);
    unsigned int                reading_uint32(RawDataParser::EndianessMode endianessMode);
    unsigned int                reading_uint16(RawDataParser::EndianessMode endianessMode);
    unsigned int                reading_uint8();
    int                         reading_int16(RawDataParser::EndianessMode endianessMode);
    string                      reading_string(int length, bool readTerminatingZero);
    void                        reading_bytes(char* buffer, int bytesNumber);

    bool                        eof();
    int                         file_size() const;
    int                         current_pos();
    int                         bytes_left();

    const RawDataParser&        get_dataParser() const;
};

class ReadableFromBin {
public:
    virtual void    read_from_bin(ReadingClass& reader) = 0;
    virtual         ~ReadableFromBin() = 0;
};

#endif // READINGCLASS_H
