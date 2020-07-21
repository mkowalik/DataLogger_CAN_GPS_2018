#ifndef WRITABLE_TO_CSV_H
#define WRITABLE_TO_CSV_H

#include "AGHUtils/WritingClass.h"

using namespace std;

class WritableToCSV {
public:
    enum class FileTimingMode {
        EventMode,
        FrameByFrameMode,
        StaticPeriod10HzMode,
        StaticPeriod100HzMode,
        StaticPeriod250HzMode,
        StaticPeriod500HzMode,
        StaticPeriod1000HzMode
    };

    virtual void    write_to_csv(FileTimingMode mode, WritingClass& writer, char decimalSeparator, bool writeOnlyChangedValues) = 0;
    virtual         ~WritableToCSV() = 0;
};

#endif // WRITABLE_TO_CSV_H
