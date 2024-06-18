// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################
#include <unity.h>
#include <realtimeclock.hpp>
#include <ctime>

extern time_t mockRealTimeClock;

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_initialize() {
    mockRealTimeClock = 0;
    realTimeClock::initialize();
    TEST_ASSERT_EQUAL(buildInfo::buildEpoch, realTimeClock::get());

    mockRealTimeClock = buildInfo::buildEpoch + 100;
    realTimeClock::initialize();
    TEST_ASSERT_EQUAL(buildInfo::buildEpoch + 100, realTimeClock::get());
}

void test_convertGpsToUnix() {
    TEST_ASSERT_EQUAL(1717101555, realTimeClock::gpsTimeToUnixTime(1401136773));
}

void test_set() {   
    mockRealTimeClock = buildInfo::buildEpoch;
    realTimeClock::set(buildInfo::buildEpoch - 100);
    TEST_ASSERT_EQUAL(buildInfo::buildEpoch, realTimeClock::get());
    realTimeClock::set(buildInfo::buildEpoch + 100);
    TEST_ASSERT_EQUAL(buildInfo::buildEpoch + 100, realTimeClock::get());
    TEST_IGNORE_MESSAGE("TODO : Further tests on Target");
}

void test_time_tToBytes() {
    time_t testInput = 0x12345678;
    uint8_t* result = realTimeClock::time_tToBytes(testInput);
    TEST_ASSERT_EQUAL(0x78, result[0]);
    TEST_ASSERT_EQUAL(0x56, result[1]);
    TEST_ASSERT_EQUAL(0x34, result[2]);
    TEST_ASSERT_EQUAL(0x12, result[3]);
}

void test_bytesToTime_t() {
    uint8_t testInput[4] = {0x78, 0x56, 0x34, 0x12};
    time_t result = realTimeClock::bytesToTime_t(testInput);
    TEST_ASSERT_EQUAL(0x12345678, result);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_convertGpsToUnix);
    RUN_TEST(test_set);
    RUN_TEST(test_time_tToBytes);
    RUN_TEST(test_bytesToTime_t);
    UNITY_END();
}
