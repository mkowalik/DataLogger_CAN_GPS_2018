#ifndef READINGCLASS_H
#define READINGCLASS_H
#include <fstream>
#include <string>

using namespace std;


class ReadingClass{
public:
    ReadingClass(string nameOfFile); //TODO dodać endianess do wyboru w argumencie
	unsigned int reading_uint32();
	unsigned int reading_uint16();
	unsigned int reading_uint8();

    int reading_int16();

    string reading_string(unsigned int length);
private:
    static const unsigned int BUFFER_SIZE = 128;
    char buffer[BUFFER_SIZE];
    ifstream data;
    unsigned int parse_little_endian(char* aNumberToParse);
    void clear_buffer(size_t length);
};

class ReadableFromBin {
public:
    virtual void read_bin(ReadingClass& reader) = 0;
};

#endif // READINGCLASS_H
