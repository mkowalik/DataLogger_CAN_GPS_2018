#ifndef RAW_DATA_PARSER_H
#define RAW_DATA_PARSER_H

class RawDataParser
{
public:
    enum EndianessMode {
        UseDefaultEndian = 1,
        LittleEndian,
        BigEndian
    };
    RawDataParser(EndianessMode rawDataMode);
    int             interpret_signed_int(char* raw_data, int bytes, EndianessMode endianessMode) const;
    unsigned int    interpret_unsigned_int(char* raw_data, int bytes, EndianessMode endianessMode) const;

    void            write_signed_int(int value, char* retBuffer, unsigned int bytes, EndianessMode endianessMode);
    void            write_unsigned_int(unsigned int value, char* retBuffer, unsigned int bytes, EndianessMode endianessMode);
private:
    EndianessMode   rawDataMode;
};

#endif // RAW_DATA_PARSER_H
