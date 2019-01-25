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
    dataParser.write_unsigned_int(aValue, buffer, 1);
    fileStream.write(buffer, 1);
}
void WritingClass::write_uint16(unsigned int aValue){
    clear_buffer(4);
    dataParser.write_unsigned_int(aValue, buffer, 4);
    fileStream.write(buffer, 2);
}
void WritingClass::write_uint32(unsigned int aValue){
    clear_buffer(4);
    dataParser.write_unsigned_int(aValue, buffer, 4);
    fileStream.write(buffer, 4);
}

void WritingClass::write_int16(int aValue){
    clear_buffer(4);
    dataParser.write_signed_int(aValue, buffer, 2);
    fileStream.write(buffer, 2);
}

void WritingClass::write_string(string aStr, bool writeTerminatingZero, int aLength){

    unsigned int bytesWritten = 0;
    unsigned int bytesLeft;

    if (aLength >= 0){
        bytesLeft = static_cast<unsigned int>(aLength);
    } else {
        bytesLeft = aStr.length();
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
void WritingClass::write_int_to_string(int value, bool writeTerminatingZero){

    clear_buffer(BUFFER_SIZE);
    sprintf (buffer, "%d", value);

    write_string(buffer, writeTerminatingZero);
}

WritableToBin::~WritableToBin(){ }

WritableToCSV::~WritableToCSV(){ }
