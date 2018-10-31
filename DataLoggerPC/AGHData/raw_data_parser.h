#ifndef RAW_DATA_PARSER_H
#define RAW_DATA_PARSER_H

class RawDataParser
{
public:
    enum EndianessMode {
        LittleEndian = 0
//        BigEndian //TODO
    };
    RawDataParser(EndianessMode rawDataMode);
    int             interpret_signed_int(char* raw_data, unsigned int bytes) const;
    unsigned int    interpret_unsigned_int(char* raw_data, unsigned int bytes) const;

    void            write_signed_int(int value, char* retBuffer, unsigned int bytes);
    void            write_unsigned_int(unsigned int value, char* retBuffer, unsigned int bytes);
private:
    EndianessMode   rawDataMode;
};

#endif // RAW_DATA_PARSER_H
