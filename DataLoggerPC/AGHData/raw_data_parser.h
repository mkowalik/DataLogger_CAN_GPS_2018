#ifndef RAW_DATA_PARSER_H
#define RAW_DATA_PARSER_H

class RawDataParser
{
public:
    enum EndianessMode {
        LittleEndian = 0,
        BigEndian
    };
    RawDataParser(EndianessMode mode);
    int             interpret_signed_int(char* raw_data, unsigned int bytes) const;
    unsigned int    interpret_unsigned_int(char* raw_data, unsigned int bytes) const;
private:
    EndianessMode   mode;
};

#endif // RAW_DATA_PARSER_H
