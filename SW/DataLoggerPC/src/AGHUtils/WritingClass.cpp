#include "WritingClass.h"
#include <cstring>
#include <cstdio>
#include <iostream>
#include <algorithm>

using namespace std;

const unsigned int WritingClass::BUFFER_SIZE;

WritingClass::WritingClass(string nameOfFile, RawDataParser& dataParser) :
    fileStream(nameOfFile.c_str(), ios_base::binary), dataParser(dataParser)
{
}

void WritingClass::write_uint8(unsigned int aValue){
    clear_buffer(4);
    dataParser.write_unsigned_int(aValue, reinterpret_cast<unsigned char*>(buffer), 1);
    fileStream.write(buffer, 1);
}

void WritingClass::write_uint16(unsigned int aValue)
{
    write_uint16(aValue, dataParser.getDefaultEndianessMode());
}

void WritingClass::write_uint32(unsigned int aValue)
{
    write_uint32(aValue, dataParser.getDefaultEndianessMode());
}

void WritingClass::write_uint64(unsigned long long aValue)
{
    write_uint64(aValue, dataParser.getDefaultEndianessMode());
}

void WritingClass::write_int16(int aValue)
{
    write_int16(aValue, dataParser.getDefaultEndianessMode());
}
void WritingClass::write_uint16(unsigned int aValue, RawDataParser::EndianessMode endianessMode){
    clear_buffer(4);
    dataParser.write_unsigned_int(aValue, reinterpret_cast<unsigned char*>(buffer), 4, endianessMode);
    fileStream.write(buffer, 2);
}
void WritingClass::write_uint32(unsigned int aValue, RawDataParser::EndianessMode endianessMode){
    clear_buffer(4);
    dataParser.write_unsigned_int(aValue, reinterpret_cast<unsigned char*>(buffer), 4, endianessMode);
    fileStream.write(buffer, 4);
}

void WritingClass::write_uint64(unsigned long long aValue, RawDataParser::EndianessMode endianessMode){
    clear_buffer(8);
    dataParser.write_unsigned_long_long(aValue, reinterpret_cast<unsigned char*>(buffer), 8, endianessMode);
    fileStream.write(buffer, 4);
}

void WritingClass::write_int16(int aValue, RawDataParser::EndianessMode endianessMode){
    clear_buffer(4);
    dataParser.write_signed_int(aValue, reinterpret_cast<unsigned char*>(buffer), 2, endianessMode);
    fileStream.write(buffer, 2);
}

void WritingClass::write_string(string aStr, bool writeTerminatingZero, int aLength){

    unsigned int bytesWritten = 0;
    unsigned int bytesLeft;

    if (aLength >= 0){
        bytesLeft = static_cast<unsigned int>(aLength);
    } else {
        bytesLeft = static_cast<unsigned int>(aStr.length());
    }

    if (writeTerminatingZero){
        bytesLeft++;
    }

    static const unsigned int BUFFER_SIZE = 128;
    while (bytesLeft > 0){
        clear_buffer(BUFFER_SIZE);
        unsigned int toCopy = min(bytesLeft, BUFFER_SIZE);
        memcpy(buffer, aStr.c_str() + bytesWritten, min(bytesLeft, static_cast<unsigned int>(aStr.length())));
        fileStream.write(buffer, static_cast<streamsize>(toCopy));
        bytesLeft -= toCopy;
        bytesWritten += toCopy;
    }
}

void WritingClass::write_char(char c){
    fileStream.put(c);
}

void WritingClass::clear_buffer(size_t length){
    memset(buffer, 0, length);
}

void WritingClass::write_double_to_string(double value, int decimal_figures, char decimalSeparator, bool writeTerminatingZero){

    if (decimalSeparator != '.' && decimalSeparator != ','){
        throw invalid_argument("Decimal separator must be '.' or ','.");
    }

    static const int MAX_DECIMAL_FIGURES = 9;

    if (decimal_figures < 0){
        decimal_figures=0;
    }

    if (decimal_figures > MAX_DECIMAL_FIGURES){
        decimal_figures = MAX_DECIMAL_FIGURES;
    }

    clear_buffer(BUFFER_SIZE);
    string format = string("%.") + to_string(decimal_figures) + string("f");
    sprintf (buffer, format.c_str(), value);
    if (decimalSeparator == ','){
        replace(buffer, buffer+strlen(buffer), '.', ',');
    }

    write_string(buffer, writeTerminatingZero);
}
void WritingClass::write_int_to_string(int value, bool writeTerminatingZero, unsigned int minSignsNumber){

    clear_buffer(BUFFER_SIZE);
    if (minSignsNumber == 0u){
        sprintf (buffer, "%d", value);
    } else {
        string format = "%0";
        format.append(to_string(minSignsNumber));
        format.append("d");
        sprintf (buffer, format.c_str(), value);
    }

    write_string(buffer, writeTerminatingZero);
}
void WritingClass::write_int_to_string(unsigned int value, bool writeTerminatingZero, unsigned int minSignsNumber){

    clear_buffer(BUFFER_SIZE);
    if (minSignsNumber == 0u){
        sprintf (buffer, "%u", value);
    } else {
        string format = "%0";
        format.append(to_string(minSignsNumber));
        format.append("u");
        sprintf (buffer, format.c_str(), value);
    }

    write_string(buffer, writeTerminatingZero);
}

void WritingClass::write_int_to_string(long long value, bool writeTerminatingZero, unsigned int minSignsNumber){

    clear_buffer(BUFFER_SIZE);
    if (minSignsNumber == 0u){
        sprintf (buffer, "%lld", value);
    } else {
        string format = "%0";
        format.append(to_string(minSignsNumber));
        format.append("ll");
        sprintf (buffer, format.c_str(), value);
    }

    write_string(buffer, writeTerminatingZero);
}

void WritingClass::write_int_to_string(unsigned long long value, bool writeTerminatingZero, unsigned int minSignsNumber){

    clear_buffer(BUFFER_SIZE);
    if (minSignsNumber == 0u){
        sprintf (buffer, "%llu", value);
    } else {
        string format = "%0";
        format.append(to_string(minSignsNumber));
        format.append("llu");
        sprintf (buffer, format.c_str(), value);
    }

    write_string(buffer, writeTerminatingZero);
}


WritableToBin::~WritableToBin(){ }

