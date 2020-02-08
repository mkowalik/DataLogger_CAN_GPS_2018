#include "AGHData/SingleCANErrorData.h"

//< ----- CANErrorCode methods ----- >//

CANErrorCode::CANErrorCode()
{
    setWordValue(0);
}

CANErrorCode::CANErrorCode(const CANErrorCode &objToCopy)
{
    setWordValue(objToCopy.getWordValue());
}

CANErrorCode::CANErrorCode(ReadingClass &reader)
{
    readFromBin(reader);
}

CANErrorCode::CANErrorCode(unsigned short _wordValue)
{
    setWordValue(_wordValue);
}

void CANErrorCode::mergeWithOther(const CANErrorCode &errorToMerge)
{
    wordValue.wrd |= (errorToMerge.getWordValue());
}

void CANErrorCode::setWordValue(unsigned short _val){
    unsigned short prev = getWordValue();
    wordValue.wrd       = _val;
    if (wordValue.s._unused != 0){
        wordValue.wrd = prev;
        throw std::invalid_argument("Raw value of CAN error code is incorrect. Some of unused bits set to 1.");
    }
    wordValue.wrd = _val;
}

void CANErrorCode::clear()
{
    setWordValue(0);
}

bool CANErrorCode::noError() const
{
    return (wordValue.wrd == 0);
}

bool CANErrorCode::isProtocolErrWarn() const
{
    return (wordValue.s.protocolErrWarn != 0);
}

bool CANErrorCode::isErrPassive() const
{
    return (wordValue.s.errPassive != 0);
}

bool CANErrorCode::isBusOff() const
{
    return (wordValue.s.busOff != 0);
}

bool CANErrorCode::isBitStuffingError() const
{
    return (wordValue.s.bitStuffingError != 0);
}

bool CANErrorCode::isFormError() const
{
    return (wordValue.s.formError != 0);
}

bool CANErrorCode::isACKError() const
{
    return (wordValue.s.ACKError != 0);
}

bool CANErrorCode::isBitRecessiveError() const
{
    return (wordValue.s.bitRecessiveError != 0);
}

bool CANErrorCode::isBitDominantError() const
{
    return (wordValue.s.bitDominantError != 0);
}

bool CANErrorCode::isCRCError() const
{
    return (wordValue.s.CRCError != 0);
}

bool CANErrorCode::isTransceiverError() const
{
    return (wordValue.s.transceiverError != 0);
}

bool CANErrorCode::isOtherError() const
{
    return (wordValue.s.otherError != 0);
}

unsigned short CANErrorCode::getWordValue() const
{
    return wordValue.wrd;
}

void CANErrorCode::readFromBin(ReadingClass &reader)
{
    setWordValue(static_cast<unsigned short>(reader.reading_uint16()));
}

CANErrorCode::~CANErrorCode()
{
}

//< ----- SingleCANErrorData methods ----- >//

SingleCANErrorData::SingleCANErrorData(const SingleCANErrorData &objToCopy) : msTime(objToCopy.getMsTime()), errorCode(objToCopy.getErrorCode())
{
}

SingleCANErrorData::SingleCANErrorData(unsigned int _msTime, ReadingClass &reader) : msTime{_msTime}, errorCode{reader}
{
}

SingleCANErrorData::SingleCANErrorData(unsigned int _msTime, CANErrorCode _errorCode) : msTime{_msTime}, errorCode{_errorCode}
{
}

unsigned int SingleCANErrorData::getMsTime() const
{
    return msTime;
}

CANErrorCode SingleCANErrorData::getErrorCode() const
{
    return errorCode;
}

void SingleCANErrorData::readFromBin(ReadingClass &reader)
{
    errorCode.readFromBin(reader);
}

SingleCANErrorData::~SingleCANErrorData()
{

}
