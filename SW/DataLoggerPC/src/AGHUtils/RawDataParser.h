#ifndef RAW_DATA_PARSER_H
#define RAW_DATA_PARSER_H

class RawDataParser
{
public:
    enum EndianessMode {
        LittleEndian,
        BigEndian
    };
    RawDataParser(EndianessMode defaultEndianessMode);

    int                 interpret_signed_int(unsigned char* raw_data, unsigned int bytesNumber) const;
    unsigned int        interpret_unsigned_int(unsigned char* raw_data, unsigned int bytesNumber) const;
    unsigned long long  interpret_unsigned_long_long(unsigned char* raw_data, unsigned int bytesNumber) const;
    int                 interpret_signed_int(unsigned char* raw_data, unsigned int bytesNumber, EndianessMode endianessMode) const;
    unsigned int        interpret_unsigned_int(unsigned char* raw_data, unsigned int bytesNumber, EndianessMode endianessMode) const;
    unsigned long long  interpret_unsigned_long_long(unsigned char* raw_data, unsigned int bytesNumber, EndianessMode endianessMode) const;

    void                write_signed_int(int value, unsigned char* retBuffer, unsigned int bytesNumber);
    void                write_unsigned_int(unsigned int value, unsigned char* retBuffer, unsigned int bytesNumber);
    void                write_unsigned_long_long(unsigned long long value, unsigned char* retBuffer, unsigned int bytes);
    void                write_signed_int(int value, unsigned char* retBuffer, unsigned int bytesNumber, EndianessMode endianessMode);
    void                write_unsigned_int(unsigned int value, unsigned char* retBuffer, unsigned int bytesNumber, EndianessMode endianessMode);
    void                write_unsigned_long_long(unsigned long long value, unsigned char* retBuffer, unsigned int bytes, EndianessMode endianessMode);

    EndianessMode       getDefaultEndianessMode();
private:
    EndianessMode       defaultEndianessMode;
};

#endif // RAW_DATA_PARSER_H
