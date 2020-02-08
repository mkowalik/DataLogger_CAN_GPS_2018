#include "CSVWriter.h"

std::string CSVWriter::errorCodeToString(CANErrorCode errorCode)
{
    std::string ret;
    if (errorCode.isProtocolErrWarn()) {
        ret = "PROTOCOL_ERROR_WARNING";
    }
    if (errorCode.isErrPassive()) {
        ret += ret.empty() ? "" : "|";
        ret += "ERROR_PASSIVE";
    }
    if (errorCode.isBusOff()) {
        ret += ret.empty() ? "" : "|";
        ret += "BUS_OFF";
    }
    if (errorCode.isBitStuffingError()) {
        ret += ret.empty() ? "" : "|";
        ret += "BIT_STUFFING_ERROR";
    }
    if (errorCode.isFormError()) {
        ret += ret.empty() ? "" : "|";
        ret += "FORM_ERROR";
    }
    if (errorCode.isACKError()) {
        ret += ret.empty() ? "" : "|";
        ret += "ACK_ERROR";
    }
    if (errorCode.isBitRecessiveError()) {
        ret += ret.empty() ? "" : "|";
        ret += "BIT_RECESSIVE_ERROR";
    }
    if (errorCode.isBitDominantError()) {
        ret += ret.empty() ? "" : "|";
        ret += "BIT_DOMINANT_ERROR";
    }
    if (errorCode.isCRCError()) {
        ret += ret.empty() ? "" : "|";
        ret += "CRC_ERROR";
    }
    if (errorCode.isTransceiverError()) {
        ret += ret.empty() ? "" : "|";
        ret += "TRANSCEIVER_ERROR";
    }
    if (errorCode.isOtherError()) {
        ret += ret.empty() ? "" : "|";
        ret += "OTHER_ERROR";
    }
    return ret;
}
