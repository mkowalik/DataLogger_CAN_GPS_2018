#ifndef FIXEDPOINT_H
#define FIXEDPOINT_H

using namespace std;

class FixedPoint
{
public:
    FixedPoint(int fractionalBits);
    FixedPoint(int valueConverted, int divider, int multiplier, int offset, int fractionalBits);
    FixedPoint(int decimalPart, unsigned int fractionalNumerator, unsigned int fractionalDenominator, int fractionalBits);
    void setRawIntegerValue(int integerValue);
    double getDoubleVal() const;
    operator double() const;
private:
    static const int MAX_FRACTIONAL_BITS_VALUE = 31;
    int fractionalBits;
    int integer;
};

#endif // FIXEDPOINT_H
