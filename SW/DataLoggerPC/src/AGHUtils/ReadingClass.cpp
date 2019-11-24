#include "ReadingClass.h"
#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

const int ReadingClass::BUFFER_SIZE;

ReadingClass::ReadingClass(string nameOfFile, RawDataParser& dataParser) : dataParser(dataParser), data(nameOfFile.c_str(), ios_base::binary) {
    clear_buffer(BUFFER_SIZE);

    streampos begin,end;

    data.seekg (0, ios::end);
    end = data.tellg();
    data.seekg (0, ios::beg);
    begin = data.tellg();

    fileSize = (end-begin);
}

unsigned int ReadingClass::reading_uint32(RawDataParser::EndianessMode endianessMode){
    if (bytes_left() < 4){
        throw out_of_range("Less than 4 bytes left while reading uint32.");
    }
    data.read(reinterpret_cast<char*>(buffer), 4);
    return dataParser.interpret_unsigned_int(buffer, 4, endianessMode);
}

unsigned int ReadingClass::reading_uint16(RawDataParser::EndianessMode endianessMode){
    if (bytes_left() < 2){
        throw out_of_range("Less than 2 bytes left while reading uint16.");
    }
    data.read(reinterpret_cast<char*>(buffer), 2);
    return dataParser.interpret_unsigned_int(buffer, 2, endianessMode);
}

unsigned int ReadingClass::reading_uint8(){
    if (bytes_left() < 1){
        throw out_of_range("Less than 1 byte left while reading uint8.");
    }
    data.read(reinterpret_cast<char*>(buffer), 1);
    return dataParser.interpret_unsigned_int(buffer, 1, RawDataParser::UseDefaultEndian);
}

int ReadingClass::reading_int16(RawDataParser::EndianessMode endianessMode){
    if (bytes_left() < 2){
        throw out_of_range("Less than 2 bytes left while reading int16.");
    }
    data.read(reinterpret_cast<char*>(buffer), 2);
    return dataParser.interpret_signed_int(buffer, 2, endianessMode);
}

string ReadingClass::reading_string(int length, bool readTerminatingZero){

    if (length < 0){
        throw invalid_argument("Length should be no less than 0.");
    }

    if (bytes_left() < length + static_cast<int>(readTerminatingZero)){
        throw out_of_range("Less than given bytes left while reading string.");
    }

    int bytesLeft = length;

    if (readTerminatingZero){
        ++bytesLeft;
    }

    string retString;
	while (bytesLeft > 0){
        data.read(reinterpret_cast<char*>(buffer), static_cast<streamsize>(min(bytesLeft, BUFFER_SIZE)));
        bytesLeft -= static_cast<unsigned int>(data.gcount());

        retString.append(reinterpret_cast<char*>(buffer), static_cast<size_t>(data.gcount()));
	}
	
	return retString;
}

void ReadingClass::clear_buffer(size_t length){
    memset(buffer, 0, length);
}

void ReadingClass::reading_bytes(char* aBuffer, int bytesNumber){

    if (bytesNumber < 0){
        throw std::invalid_argument("bytesNumber can't be less than zero.");
    }

    if (bytes_left() < bytesNumber){
        throw out_of_range("Less than given bytes left while reading consecutive bytes.");
    }

    int bytesLeft = bytesNumber;
    int bytesRead = 0;

    while (bytesLeft > 0){
        data.read(aBuffer, static_cast<streamsize>(bytesLeft));
        bytesLeft -= static_cast<unsigned int>(data.gcount());
        bytesRead += static_cast<unsigned int>(data.gcount());
    }
}

bool ReadingClass::eof(){
    return (bytes_left() == 0);
}

streamoff ReadingClass::file_size() const {
    return fileSize;
}

streamoff ReadingClass::current_pos() {
    return data.tellg();
}

streamoff ReadingClass::bytes_left() {
    return fileSize - current_pos();
}

const RawDataParser& ReadingClass::get_dataParser() const {
    return const_cast<RawDataParser&>(dataParser);
}

ReadableFromBin::~ReadableFromBin(){ }

