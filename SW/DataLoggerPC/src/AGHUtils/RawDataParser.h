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

    int                 interpret_signed_int(unsigned char* raw_data, unsigned int bytesNumber, EndianessMode endianessMode) const;
    unsigned int        interpret_unsigned_int(unsigned char* raw_data, unsigned int bytesNumber, EndianessMode endianessMode) const;
    unsigned long long  interpret_unsigned_long_long(unsigned char* raw_data, unsigned int bytesNumber, EndianessMode endianessMode) const;

    void                write_signed_int(int value, unsigned char* retBuffer, unsigned int bytesNumber, EndianessMode endianessMode);
    void                write_unsigned_int(unsigned int value, unsigned char* retBuffer, unsigned int bytesNumber, EndianessMode endianessMode);
    void                write_unsigned_long_long(unsigned long long value, unsigned char* retBuffer, unsigned int bytes, EndianessMode endianessMode);
private:
    EndianessMode   rawDataMode;
};

#endif // RAW_DATA_PARSER_H
