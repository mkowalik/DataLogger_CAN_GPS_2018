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
    Config* pConfig;
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

    pConfig = new Config("", Config::EnCANBitrate::bitrate_500kbps, Config::EnGPSFrequency::freq_GPS_OFF, 0x7FF, false, 0);
    pConfigFrame = pConfig->addFrame(0x100, 4, "testFrame");


    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame, rawData, 4);
    pSignal = pConfigFrame->addSignal(4, 8, ValueType(false, false), 1, 1, 0, "testSignal", "", "");

    unsigned long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0x51u);

    delete pConfig;
    delete pTestSingleCANFrameData;
}

void Test_SingleCANFrameData::test_case_littleEndian_2()
{
    pConfig = new Config("", Config::EnCANBitrate::bitrate_500kbps, Config::EnGPSFrequency::freq_GPS_OFF, 0x7FF, false, 0);
    pConfigFrame = pConfig->addFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame, rawData, 4);
    pSignal = pConfigFrame->addSignal(4, 4, ValueType(false, false), 1, 1, 0, "testSignal", "", "");

    unsigned long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0x5u);

    delete pTestSingleCANFrameData;
    delete pConfig;
}

void Test_SingleCANFrameData::test_case_littleEndian_3()
{
    pConfig = new Config("", Config::EnCANBitrate::bitrate_500kbps, Config::EnGPSFrequency::freq_GPS_OFF, 0x7FF, false, 0);
    pConfigFrame = pConfig->addFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame, rawData, 4);
    pSignal = pConfigFrame->addSignal(4, 16, ValueType(false, false), 1, 1, 0, "testSignal", "", "");


    unsigned long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0x6A51u);

    delete pTestSingleCANFrameData;
    delete pConfig;
}

void Test_SingleCANFrameData::test_case_littleEndian_4()
{
    pConfig = new Config("", Config::EnCANBitrate::bitrate_500kbps, Config::EnGPSFrequency::freq_GPS_OFF, 0x7FF, false, 0);
    pConfigFrame = pConfig->addFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame, rawData, 4);
    pSignal = pConfigFrame->addSignal(4, 12, ValueType(false, false), 1, 1, 0, "testSignal", "", "");

    unsigned long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0x651u);

    delete pTestSingleCANFrameData;
    delete pConfig;
}

void Test_SingleCANFrameData::test_case_littleEndian_5()
{
    pConfig = new Config("", Config::EnCANBitrate::bitrate_500kbps, Config::EnGPSFrequency::freq_GPS_OFF, 0x7FF, false, 0);
    pConfigFrame = pConfig->addFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame, rawData, 4);
    pSignal = pConfigFrame->addSignal(5, 10, ValueType(false, false), 1, 1, 0, "testSignal", "", "");

    unsigned long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0b1110100010u);

    delete pTestSingleCANFrameData;
    delete pConfig;
}

void Test_SingleCANFrameData::test_case_littleEndian_6()
{
    pConfig = new Config("", Config::EnCANBitrate::bitrate_500kbps, Config::EnGPSFrequency::freq_GPS_OFF, 0x7FF, false, 0);
    pConfigFrame = pConfig->addFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame, rawData, 4);
    pSignal = pConfigFrame->addSignal(0, 32, ValueType(false, false), 1, 1, 0, "testSignal", "", "");

    unsigned long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0x3AA31615u);

    delete pTestSingleCANFrameData;
    delete pConfig;
}

void Test_SingleCANFrameData::test_case_littleEndian_7()
{
    pConfig = new Config("", Config::EnCANBitrate::bitrate_500kbps, Config::EnGPSFrequency::freq_GPS_OFF, 0x7FF, false, 0);
    pConfigFrame = pConfig->addFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame, rawData, 4);
    pSignal = pConfigFrame->addSignal(2, 2, ValueType(false, false), 1, 1, 0, "testSignal", "", "");

    unsigned long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0b01u);

    delete pTestSingleCANFrameData;
    delete pConfig;
}

void Test_SingleCANFrameData::test_case_littleEndian_8()
{
    pConfig = new Config("", Config::EnCANBitrate::bitrate_500kbps, Config::EnGPSFrequency::freq_GPS_OFF, 0x7FF, false, 0);
    pConfigFrame = pConfig->addFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame, rawData, 4);
    pSignal = pConfigFrame->addSignal(7, 2, ValueType(false, false), 1, 1, 0, "testSignal", "", "");

    unsigned long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0b10u);

    delete pTestSingleCANFrameData;
    delete pConfig;
}


void Test_SingleCANFrameData::test_case_bigEndian_1()
{
    pConfig = new Config("", Config::EnCANBitrate::bitrate_500kbps, Config::EnGPSFrequency::freq_GPS_OFF, 0x7FF, false, 0);
    pConfigFrame = pConfig->addFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame, rawData, 4);
    pSignal = pConfigFrame->addSignal(4, 8, ValueType(false, true), 1, 1, 0, "testSignal", "", "");

    unsigned long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0x51u);

    delete pTestSingleCANFrameData;
    delete pConfig;
}

void Test_SingleCANFrameData::test_case_bigEndian_2()
{
    pConfig = new Config("", Config::EnCANBitrate::bitrate_500kbps, Config::EnGPSFrequency::freq_GPS_OFF, 0x7FF, false, 0);
    pConfigFrame = pConfig->addFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame, rawData, 4);
    pSignal = pConfigFrame->addSignal(4, 16, ValueType(false, true), 1, 1, 0, "testSignal", "", "");

    unsigned long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0x516Au);

    delete pTestSingleCANFrameData;
    delete pConfig;
}

void Test_SingleCANFrameData::test_case_bigEndian_3(){
    pConfig = new Config("", Config::EnCANBitrate::bitrate_500kbps, Config::EnGPSFrequency::freq_GPS_OFF, 0x7FF, false, 0);
    pConfigFrame = pConfig->addFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame, rawData, 4);
    pSignal = pConfigFrame->addSignal(4, 16, ValueType(false, true), 1, 1, 0, "testSignal", "", "");

    unsigned long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0x516Au);

    delete pTestSingleCANFrameData;
    delete pConfig;
}

void Test_SingleCANFrameData::test_case_bigEndian_4()
{
    pConfig = new Config("", Config::EnCANBitrate::bitrate_500kbps, Config::EnGPSFrequency::freq_GPS_OFF, 0x7FF, false, 0);
    pConfigFrame = pConfig->addFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame, rawData, 4);
    pSignal = pConfigFrame->addSignal(4, 12, ValueType(false, true), 1, 1, 0, "testSignal", "", "");

    unsigned long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0x516u);

    delete pTestSingleCANFrameData;
    delete pConfig;
}

void Test_SingleCANFrameData::test_case_bigEndian_5()
{
    pConfig = new Config("", Config::EnCANBitrate::bitrate_500kbps, Config::EnGPSFrequency::freq_GPS_OFF, 0x7FF, false, 0);
    pConfigFrame = pConfig->addFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame, rawData, 4);
    pSignal = pConfigFrame->addSignal(5, 10, ValueType(false, true), 1, 1, 0, "testSignal", "", "");

    unsigned long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0b1010001011u);

    delete pTestSingleCANFrameData;
    delete pConfig;
}

void Test_SingleCANFrameData::test_case_bigEndian_6()
{
    pConfig = new Config("", Config::EnCANBitrate::bitrate_500kbps, Config::EnGPSFrequency::freq_GPS_OFF, 0x7FF, false, 0);
    pConfigFrame = pConfig->addFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame, rawData, 4);
    pSignal = pConfigFrame->addSignal(0, 32, ValueType(false, true), 1, 1, 0, "testSignal", "", "");

    unsigned long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0x1516A33Au);

    delete pTestSingleCANFrameData;
    delete pConfig;
}

void Test_SingleCANFrameData::test_case_bigEndian_7()
{
    pConfig = new Config("", Config::EnCANBitrate::bitrate_500kbps, Config::EnGPSFrequency::freq_GPS_OFF, 0x7FF, false, 0);
    pConfigFrame = pConfig->addFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame, rawData, 4);
    pSignal = pConfigFrame->addSignal(2, 2, ValueType(false, true), 1, 1, 0, "testSignal", "", "");

    unsigned long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0b01u);

    delete pTestSingleCANFrameData;
    delete pConfig;
}

void Test_SingleCANFrameData::test_case_bigEndian_8()
{
    pConfig = new Config("", Config::EnCANBitrate::bitrate_500kbps, Config::EnGPSFrequency::freq_GPS_OFF, 0x7FF, false, 0);
    pConfigFrame = pConfig->addFrame(0x100, 4, "testFrame");

    unsigned char rawData[4] = {0x15, 0x16, 0xA3, 0x3A};

    pTestSingleCANFrameData = new SingleCANFrameData(100, pConfigFrame, rawData, 4);
    pSignal = pConfigFrame->addSignal(7, 2, ValueType(false, true), 1, 1, 0, "testSignal", "", "");

    unsigned long signalVal = pTestSingleCANFrameData->getSignalValueRaw(pSignal);
    QCOMPARE(signalVal, 0b10u);

    delete pTestSingleCANFrameData;
    delete pConfig;
}

QTEST_MAIN(Test_SingleCANFrameData)

#include "tst_singlecanframedata.moc"
