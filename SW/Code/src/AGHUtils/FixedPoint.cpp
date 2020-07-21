#include "FixedPoint.h"

#include <stdexcept>

using namespace std;

FixedPoint::FixedPoint(int fractionalBits) : fractionalBits(fractionalBits), integer(0){
    if (fractionalBits > FixedPoint::MAX_FRACTIONAL_BITS_VALUE){
        throw std::invalid_argument("fractionaBits valeu exeeds MAX_FRACTIONAL_BITS_VALUE value.");
    }
}

FixedPoint::FixedPoint(int valueConverted, int divider, int multiplier, int offset, int fractionalBits) : fractionalBits(fractionalBits) {
    if (fractionalBits > FixedPoint::MAX_FRACTIONAL_BITS_VALUE){
        throw std::invalid_argument("fractionaBits valeu exeeds MAX_FRACTIONAL_BITS_VALUE value.");
    }
    valueConverted -= offset;
    integer =  ((valueConverted * divider) / multiplier) << fractionalBits;
    integer |= (((valueConverted * divider) % multiplier) * (1<<fractionalBits)) / multiplier;
}
FixedPoint::FixedPoint(int decimalPart, unsigned int fractionalNumerator, unsigned int fractionalDenominator, int fractionalBits) : fractionalBits(fractionalBits) {
    if (fractionalBits > FixedPoint::MAX_FRACTIONAL_BITS_VALUE){
        throw std::invalid_argument("fractionaBits valeu exeeds MAX_FRACTIONAL_BITS_VALUE value.");
    }
    decimalPart += fractionalNumerator / fractionalDenominator; //< handle case when fractionalNumerator > fractionalDenomianator
    fractionalNumerator %= fractionalDenominator;

    integer  = decimalPart << fractionalBits;
    integer |= (fractionalNumerator << fractionalBits) / fractionalDenominator;
}
void FixedPoint::setRawIntegerValue(int integerValue) {
    integer = integerValue;
}
double FixedPoint::getDoubleVal() const {
    double ret = static_cast<double>(integer >> fractionalBits);
    double numerator = static_cast<double>(integer & ((1<<fractionalBits)-1));
    double denominator = static_cast<double>(1<<fractionalBits);
    ret += numerator/denominator;
    return ret;
}
FixedPoint::operator double() const {
    return getDoubleVal();
}
