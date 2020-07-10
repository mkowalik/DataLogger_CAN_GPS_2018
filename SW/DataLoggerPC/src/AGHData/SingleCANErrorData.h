#ifndef SINGLECANERRORDATA_H
#define SINGLECANERRORDATA_H

#include <AGHUtils/ReadingClass.h>

class CANErrorCode : ReadableFromBin {
private:
    union MaskErrorCode {
        struct {
            unsigned short protocolErrWarn      : 1;
            unsigned short errPassive           : 1;
            unsigned short busOff               : 1;
            unsigned short bitStuffingError     : 1;
            unsigned short formError            : 1;
            unsigned short ACKError             : 1;
            unsigned short bitRecessiveError    : 1;
            unsigned short bitDominantError     : 1;
            unsigned short CRCError             : 1;
            unsigned short transceiverError     : 1;
            unsigned short otherError           : 1;
            unsigned short _unused              : 5;
        } s;
        unsigned short wrd;
    } wordValue;
public:
    CANErrorCode();
    CANErrorCode(const CANErrorCode& objToCopy);
    CANErrorCode(ReadingClass &reader);
    CANErrorCode(unsigned short wordValue);

    void mergeWithOther(const CANErrorCode& errorToMerge);
    void setWordValue(unsigned short _val);
    void clear();

    bool noError() const;

    bool isProtocolErrWarn() const;
    bool isErrPassive() const;
    bool isBusOff() const;
    bool isBitStuffingError() const;
    bool isFormError() const;
    bool isACKError() const;
    bool isBitRecessiveError() const;
    bool isBitDominantError() const;
    bool isCRCError() const;
    bool isTransceiverError() const;
    bool isOtherError() const;

    unsigned short getWordValue() const;
    // ReadableFromBin interface
    virtual void readFromBin(ReadingClass &reader) override;

    virtual ~CANErrorCode() override;
};

class SingleCANErrorData : public ReadableFromBin
{
private:
    unsigned int    msTime;
    CANErrorCode    errorCode;
public:
    SingleCANErrorData(const SingleCANErrorData& objToCopy);
    SingleCANErrorData(unsigned int msTime, ReadingClass& reader);
    SingleCANErrorData(unsigned int msTime, CANErrorCode errorCode);

    unsigned int    getMsTime() const;
    CANErrorCode    getErrorCode() const;

    // ReadableFromBin interface
    virtual void readFromBin(ReadingClass &reader) override;

    virtual ~SingleCANErrorData() override;
};

#endif // SINGLECANERRORDATA_H
