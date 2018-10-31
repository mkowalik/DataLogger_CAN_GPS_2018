#ifndef READINGCLASS_H
#define READINGCLASS_H
#include <fstream>
#include <string>
#include "AGHData/raw_data_parser.h"

using namespace std;


class ReadingClass{
private:
    static const unsigned int   BUFFER_SIZE = 128;
    RawDataParser&              dataParser;
    char                        buffer[BUFFER_SIZE];
    ifstream                    data;

    void                clear_buffer(size_t length);
public:
    ReadingClass(string nameOfFile, RawDataParser& dataParser);
    unsigned int         reading_uint32();
    unsigned int         reading_uint16();
    unsigned int         reading_uint8();
    int                  reading_int16();
    string               reading_string(unsigned int length);
    void                 reading_bytes(char* buffer, unsigned int bytesNumber);

    bool                 eof() const;

    const RawDataParser& get_dataParser() const;
};

class ReadableFromBin {
public:
    virtual void    read_from_bin(ReadingClass& reader) = 0;
    virtual         ~ReadableFromBin() = 0;
};

#endif // READINGCLASS_H
