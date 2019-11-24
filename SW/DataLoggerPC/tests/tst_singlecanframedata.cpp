#include <QtTest>
#include <QCoreApplication>

// add necessary includes here

#include "AGHConfig/Config.h"
#include "AGHConfig/ConfigSignal.h"
#include "AGHData/SingleCANFrameData.h"

class Test_SingleCANFrameData : public QObject
{
    Q_OBJECT
private:
    ConfigFrame* pConfigFrame;
    ConfigSignal* pSignal;
    SingleCANFrameData* pTestSingleCANFrameData;

public:
    Test_SingleCANFrameData();
    ~Test_SingleCANFrameData();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case_littleEndian_1();
    void test_case_littleEndian_2();
    void test_case_littleEndian_3();
    void test_case_littleEndian_4();
    void test_case_littleEndian_5();
    void test_case_littleEndian_6();
    void test_case_littleEndian_7();
    void test_case_littleEndian_8();

    void test_case_bigEndian_1();
    void test_case_bigEndian_2();
    void test_case_bigEndian_3();
    void test_case_bigEndian_4();
    void test_case_bigEndian_5();
    void test_case_bigEndian_6();
    void test_case_bigEndian_7();
    void test_case_bigEndian_8();
};

Test_SingleCANFrameData::Test_SingleCANFrameData()
{

}

Test_SingleCANFrameData::~Test_SingleCANFrameData()
{

}

void Test_SingleCANFrameData::initTestCase()
{

}

void Test_SingleCANFrameData::cleanupTestCase(){

}

void Test_SingleCANFrameData::test_case_littleEndian_1()
{
    pConfigFrame = new ConfigFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame);
    pTestSingleCANFrameData->setRawData(rawData);

    pSignal = new ConfigSignal(pConfigFrame);
    pSignal->setStartBit(4);
    pSignal->setLengthBits(8);

    unsigned long long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0x51u);

    delete pConfigFrame;
    delete pTestSingleCANFrameData;
}

void Test_SingleCANFrameData::test_case_littleEndian_2()
{
    pConfigFrame = new ConfigFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame);
    pTestSingleCANFrameData->setRawData(rawData);

    pSignal = new ConfigSignal(pConfigFrame);
    pSignal->setStartBit(4);
    pSignal->setLengthBits(4);
    unsigned long long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0x5u);

    delete pTestSingleCANFrameData;
    delete pConfigFrame;
}

void Test_SingleCANFrameData::test_case_littleEndian_3()
{
    pConfigFrame = new ConfigFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame);
    pTestSingleCANFrameData->setRawData(rawData);

    pSignal = new ConfigSignal(pConfigFrame);
    pSignal->setStartBit(4);
    pSignal->setLengthBits(16);
    unsigned long long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0x6A51u);

    delete pTestSingleCANFrameData;
    delete pConfigFrame;
}

void Test_SingleCANFrameData::test_case_littleEndian_4()
{
    pConfigFrame = new ConfigFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame);
    pTestSingleCANFrameData->setRawData(rawData);

    pSignal = new ConfigSignal(pConfigFrame);
    pSignal->setStartBit(4);
    pSignal->setLengthBits(12);
    unsigned long long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0x651u);

    delete pTestSingleCANFrameData;
    delete pConfigFrame;
}

void Test_SingleCANFrameData::test_case_littleEndian_5()
{
    pConfigFrame = new ConfigFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame);
    pTestSingleCANFrameData->setRawData(rawData);

    pSignal = new ConfigSignal(pConfigFrame);
    pSignal->setStartBit(5);
    pSignal->setLengthBits(10);
    unsigned long long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0b1110100010u);

    delete pTestSingleCANFrameData;
    delete pConfigFrame;
}

void Test_SingleCANFrameData::test_case_littleEndian_6()
{
    pConfigFrame = new ConfigFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame);
    pTestSingleCANFrameData->setRawData(rawData);

    pSignal = new ConfigSignal(pConfigFrame);
    pSignal->setStartBit(0);
    pSignal->setLengthBits(32);
    unsigned long long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0x3AA31615u);

    delete pTestSingleCANFrameData;
    delete pConfigFrame;
}

void Test_SingleCANFrameData::test_case_littleEndian_7()
{
    pConfigFrame = new ConfigFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame);
    pTestSingleCANFrameData->setRawData(rawData);

    pSignal = new ConfigSignal(pConfigFrame);
    pSignal->setStartBit(2);
    pSignal->setLengthBits(2);
    unsigned long long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0b01u);

    delete pTestSingleCANFrameData;
    delete pConfigFrame;
}

void Test_SingleCANFrameData::test_case_littleEndian_8()
{
    pConfigFrame = new ConfigFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame);
    pTestSingleCANFrameData->setRawData(rawData);

    pSignal = new ConfigSignal(pConfigFrame);
    pSignal->setStartBit(7);
    pSignal->setLengthBits(2);
    unsigned long long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0b10u);

    delete pTestSingleCANFrameData;
    delete pConfigFrame;
}


void Test_SingleCANFrameData::test_case_bigEndian_1()
{
    pConfigFrame = new ConfigFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame);
    pTestSingleCANFrameData->setRawData(rawData);

    pSignal = new ConfigSignal(pConfigFrame);
    pSignal->setValueType(ValueType(false, true, false, false, false, true));
    pSignal->setStartBit(4);
    pSignal->setLengthBits(8);
    unsigned long long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0x51u);

    delete pTestSingleCANFrameData;
    delete pConfigFrame;
}

void Test_SingleCANFrameData::test_case_bigEndian_2()
{
    pConfigFrame = new ConfigFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame);
    pTestSingleCANFrameData->setRawData(rawData);

    pSignal = new ConfigSignal(pConfigFrame);
    pSignal->setValueType(ValueType(false, true, false, false, false, true));
    pSignal->setStartBit(4);
    pSignal->setLengthBits(16);
    unsigned long long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0x516Au);

    delete pTestSingleCANFrameData;
    delete pConfigFrame;
}

void Test_SingleCANFrameData::test_case_bigEndian_3(){
    pConfigFrame = new ConfigFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame);
    pTestSingleCANFrameData->setRawData(rawData);

    pSignal = new ConfigSignal(pConfigFrame);
    pSignal->setValueType(ValueType(false, true, false, false, false, true));
    pSignal->setStartBit(4);
    pSignal->setLengthBits(16);
    unsigned long long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0x516Au);

    delete pTestSingleCANFrameData;
    delete pConfigFrame;
}

void Test_SingleCANFrameData::test_case_bigEndian_4()
{
    pConfigFrame = new ConfigFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame);
    pTestSingleCANFrameData->setRawData(rawData);

    pSignal = new ConfigSignal(pConfigFrame);
    pSignal->setValueType(ValueType(false, true, false, false, false, true));
    pSignal->setStartBit(4);
    pSignal->setLengthBits(12);
    unsigned long long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0x516u);

    delete pTestSingleCANFrameData;
    delete pConfigFrame;
}

void Test_SingleCANFrameData::test_case_bigEndian_5()
{
    pConfigFrame = new ConfigFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame);
    pTestSingleCANFrameData->setRawData(rawData);

    pSignal = new ConfigSignal(pConfigFrame);
    pSignal->setValueType(ValueType(false, true, false, false, false, true));
    pSignal->setStartBit(5);
    pSignal->setLengthBits(10);
    unsigned long long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0b1010001011u);

    delete pTestSingleCANFrameData;
    delete pConfigFrame;
}

void Test_SingleCANFrameData::test_case_bigEndian_6()
{
    pConfigFrame = new ConfigFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame);
    pTestSingleCANFrameData->setRawData(rawData);

    pSignal = new ConfigSignal(pConfigFrame);
    pSignal->setValueType(ValueType(false, true, false, false, false, true));
    pSignal->setStartBit(0);
    pSignal->setLengthBits(32);
    unsigned long long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0x1516A33Au);

    delete pTestSingleCANFrameData;
    delete pConfigFrame;
}

void Test_SingleCANFrameData::test_case_bigEndian_7()
{
    pConfigFrame = new ConfigFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame);
    pTestSingleCANFrameData->setRawData(rawData);

    pSignal = new ConfigSignal(pConfigFrame);
    pSignal->setValueType(ValueType(false, true, false, false, false, true));
    pSignal->setStartBit(2);
    pSignal->setLengthBits(2);
    unsigned long long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0b01u);

    delete pTestSingleCANFrameData;
    delete pConfigFrame;
}

void Test_SingleCANFrameData::test_case_bigEndian_8()
{
    pConfigFrame = new ConfigFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame);
    pTestSingleCANFrameData->setRawData(rawData);

    pSignal = new ConfigSignal(pConfigFrame);
    pSignal->setValueType(ValueType(false, true, false, false, false, true));
    pSignal->setStartBit(7);
    pSignal->setLengthBits(2);
    unsigned long long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0b10u);

    delete pTestSingleCANFrameData;
    delete pConfigFrame;
}

QTEST_MAIN(Test_SingleCANFrameData)

#include "tst_singlecanframedata.moc"
