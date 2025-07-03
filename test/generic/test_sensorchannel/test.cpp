#include <unity.h>
#include <sensorchannel.hpp>

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_initialize() {
    sensorChannel testChannel = {0, "", ""};
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.oversampling);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.prescaling);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.prescaleCounter);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.oversamplingCounter);

    for (uint32_t index = 0; index < sensorChannel::maxOversampling; index++) {
        TEST_ASSERT_EQUAL_FLOAT(0.0F, testChannel.samples[index]);
    }
}

void test_set() {
    sensorChannel testChannel = {0, "", ""};

    testChannel.set(1, 2);
    TEST_ASSERT_EQUAL_UINT32(1, testChannel.oversampling);
    TEST_ASSERT_EQUAL_UINT32(2, testChannel.prescaling);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.oversamplingCounter);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.prescaleCounter);
}

void test_setAndRestrict() {
    sensorChannel testChannel = {0, "", ""};
    testChannel.set(testChannel.maxOversampling + 1, testChannel.maxPrescaler + 1);
    TEST_ASSERT_EQUAL_UINT32(testChannel.maxOversampling, testChannel.oversampling);
    TEST_ASSERT_EQUAL_UINT32(testChannel.maxPrescaler, testChannel.prescaling);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.oversamplingCounter);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.prescaleCounter);
}

void test_getNextAction() {
    sensorChannel testChannel = {0, "", ""};
    TEST_ASSERT_EQUAL(sensorChannel::action::none, testChannel.getNextAction());

    testChannel.set(2, 2);

    testChannel.prescaleCounter     = 1;
    testChannel.oversamplingCounter = 0;
    TEST_ASSERT_EQUAL(sensorChannel::action::prescale, testChannel.getNextAction());
    TEST_ASSERT_FALSE(testChannel.needsSampling());
    TEST_ASSERT_FALSE(testChannel.hasOutput());

    testChannel.prescaleCounter     = 1;
    testChannel.oversamplingCounter = 1;
    TEST_ASSERT_EQUAL(sensorChannel::action::prescale, testChannel.getNextAction());
    TEST_ASSERT_FALSE(testChannel.needsSampling());
    TEST_ASSERT_FALSE(testChannel.hasOutput());

    testChannel.prescaleCounter     = 0;
    testChannel.oversamplingCounter = 1;
    TEST_ASSERT_EQUAL(sensorChannel::action::sample, testChannel.getNextAction());
    TEST_ASSERT_TRUE(testChannel.needsSampling());
    TEST_ASSERT_FALSE(testChannel.hasOutput());

    testChannel.prescaleCounter     = 0;
    testChannel.oversamplingCounter = 0;
    TEST_ASSERT_EQUAL(sensorChannel::action::sampleAndOutput, testChannel.getNextAction());
    TEST_ASSERT_TRUE(testChannel.needsSampling());
    TEST_ASSERT_TRUE(testChannel.hasOutput());
}

void test_updateCounters() {
    sensorChannel testChannel = {0, "", ""};
    testChannel.set(1, 4);        // = oversampling = 1 -> average 2 samples into a measurement, prescaling = 4 -> take a sample every 4th RTC tick

    TEST_ASSERT_EQUAL_UINT32(0, testChannel.oversamplingCounter);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.prescaleCounter);
    TEST_ASSERT_EQUAL(sensorChannel::action::sampleAndOutput, testChannel.getNextAction());
    testChannel.updateCounters();
    TEST_ASSERT_EQUAL_UINT32(1, testChannel.oversamplingCounter);
    TEST_ASSERT_EQUAL_UINT32(3, testChannel.prescaleCounter);
    TEST_ASSERT_EQUAL(sensorChannel::action::prescale, testChannel.getNextAction());
    testChannel.updateCounters();
    TEST_ASSERT_EQUAL_UINT32(1, testChannel.oversamplingCounter);
    TEST_ASSERT_EQUAL_UINT32(2, testChannel.prescaleCounter);
    TEST_ASSERT_EQUAL(sensorChannel::action::prescale, testChannel.getNextAction());
    testChannel.updateCounters();
    TEST_ASSERT_EQUAL_UINT32(1, testChannel.oversamplingCounter);
    TEST_ASSERT_EQUAL_UINT32(1, testChannel.prescaleCounter);
    TEST_ASSERT_EQUAL(sensorChannel::action::prescale, testChannel.getNextAction());
    testChannel.updateCounters();
    TEST_ASSERT_EQUAL_UINT32(1, testChannel.oversamplingCounter);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.prescaleCounter);
    TEST_ASSERT_EQUAL(sensorChannel::action::sample, testChannel.getNextAction());
    testChannel.updateCounters();
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.oversamplingCounter);
    TEST_ASSERT_EQUAL_UINT32(3, testChannel.prescaleCounter);
    TEST_ASSERT_EQUAL(sensorChannel::action::prescale, testChannel.getNextAction());
    testChannel.updateCounters();
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.oversamplingCounter);
    TEST_ASSERT_EQUAL_UINT32(2, testChannel.prescaleCounter);
    TEST_ASSERT_EQUAL(sensorChannel::action::prescale, testChannel.getNextAction());
    testChannel.updateCounters();
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.oversamplingCounter);
    TEST_ASSERT_EQUAL_UINT32(1, testChannel.prescaleCounter);
    TEST_ASSERT_EQUAL(sensorChannel::action::prescale, testChannel.getNextAction());
    testChannel.updateCounters();
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.oversamplingCounter);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.prescaleCounter);
    TEST_ASSERT_EQUAL(sensorChannel::action::sampleAndOutput, testChannel.getNextAction());
    testChannel.updateCounters();
    TEST_ASSERT_EQUAL_UINT32(1, testChannel.oversamplingCounter);
    TEST_ASSERT_EQUAL_UINT32(3, testChannel.prescaleCounter);
}

void test_addSample() {
    sensorChannel testChannel = {0, "", ""};        // = oversampling = 3 + 1 = 4, prescaling = 1 (= no prescaling)
    testChannel.set(3, 1);
    testChannel.addSample(13.3F);
    TEST_ASSERT_EQUAL_FLOAT(13.3F, testChannel.samples[0]);
    testChannel.updateCounters();
    testChannel.addSample(12.2F);
    TEST_ASSERT_EQUAL_FLOAT(12.2F, testChannel.samples[3]);
    testChannel.updateCounters();
    testChannel.addSample(11.1F);
    TEST_ASSERT_EQUAL_FLOAT(11.1F, testChannel.samples[2]);
    testChannel.updateCounters();
    testChannel.addSample(10.0F);
    TEST_ASSERT_EQUAL_FLOAT(10.0F, testChannel.samples[1]);
}

void test_getOutput() {
    sensorChannel testChannel = {0, "", ""};        // = oversampling = 3 + 1 = 4, prescaling = 1 (= no prescaling)
    testChannel.set(3, 1);
    testChannel.updateCounters();
    testChannel.addSample(1.0F);
    testChannel.updateCounters();
    testChannel.addSample(2.0F);
    testChannel.updateCounters();
    testChannel.addSample(3.0F);
    testChannel.updateCounters();
    testChannel.addSample(4.0F);

    TEST_ASSERT_EQUAL_FLOAT(2.5F, testChannel.value());

    testChannel.updateCounters();
    testChannel.addSample(10.0F);
    testChannel.updateCounters();
    testChannel.addSample(20.0F);
    testChannel.updateCounters();
    testChannel.addSample(30.0F);
    testChannel.updateCounters();
    testChannel.addSample(40.0F);

    TEST_ASSERT_EQUAL_FLOAT(25.0F, testChannel.value());
}



void test_isActive() {
    sensorChannel testChannel = {0, "", ""};
    testChannel.set(1, 1);
    TEST_ASSERT_TRUE(testChannel.isActive());
    testChannel.set(0, 0);
    TEST_ASSERT_FALSE(testChannel.isActive());
}

void test_getNumberOfSamplesToAverage() {
    sensorChannel testChannel                 = {0, "", ""};
    uint32_t expectedNumberOfSamplesToAverage = 1;
    testChannel.set(sensorChannel::calculateOversampling(expectedNumberOfSamplesToAverage), sensorChannel::calculatePrescaler(10, expectedNumberOfSamplesToAverage));
    TEST_ASSERT_EQUAL_UINT32(expectedNumberOfSamplesToAverage, testChannel.getNumberOfSamplesToAverage());
    expectedNumberOfSamplesToAverage = 4;
    testChannel.set(sensorChannel::calculateOversampling(expectedNumberOfSamplesToAverage), sensorChannel::calculatePrescaler(10, expectedNumberOfSamplesToAverage));
    TEST_ASSERT_EQUAL_UINT32(expectedNumberOfSamplesToAverage, testChannel.getNumberOfSamplesToAverage());
}

void test_getMinutesBetweenOutput() {
    sensorChannel testChannel             = {0, "", ""};
    uint32_t expectedMinutesBetweenOutput = 1;
    testChannel.set(sensorChannel::calculateOversampling(1), sensorChannel::calculatePrescaler(expectedMinutesBetweenOutput, 1));
    TEST_ASSERT_EQUAL_UINT32(expectedMinutesBetweenOutput, testChannel.getMinutesBetweenOutput());

    expectedMinutesBetweenOutput = 4;
    testChannel.set(sensorChannel::calculateOversampling(1), sensorChannel::calculatePrescaler(expectedMinutesBetweenOutput, 1));
    TEST_ASSERT_EQUAL_UINT32(expectedMinutesBetweenOutput, testChannel.getMinutesBetweenOutput());

    expectedMinutesBetweenOutput = 2;
    testChannel.set(sensorChannel::calculateOversampling(2), sensorChannel::calculatePrescaler(expectedMinutesBetweenOutput, 2));
    TEST_ASSERT_EQUAL_UINT32(expectedMinutesBetweenOutput, testChannel.getMinutesBetweenOutput());

    expectedMinutesBetweenOutput = 60;
    testChannel.set(sensorChannel::calculateOversampling(8), sensorChannel::calculatePrescaler(expectedMinutesBetweenOutput, 8));
    TEST_ASSERT_EQUAL_UINT32(expectedMinutesBetweenOutput, testChannel.getMinutesBetweenOutput());
}

void test_compress_and_extract() {
    TEST_ASSERT_EQUAL_UINT8(0, sensorChannel::compressOversamplingAndPrescalerIndex(0, 0));                                                                                                                                                     // minimal values
    TEST_ASSERT_EQUAL_UINT8((sensorChannel::maxOversamplingIndex << 4) + sensorChannel::maxPrescalerIndex, sensorChannel::compressOversamplingAndPrescalerIndex(sensorChannel::maxOversamplingIndex, sensorChannel::maxPrescalerIndex));        // maximal values
    TEST_ASSERT_EQUAL_UINT8(sensorChannel::maxPrescalerIndex, sensorChannel::compressOversamplingAndPrescalerIndex(0, sensorChannel::maxPrescalerIndex));
    TEST_ASSERT_EQUAL_UINT8(sensorChannel::maxOversamplingIndex << 4, sensorChannel::compressOversamplingAndPrescalerIndex(sensorChannel::maxOversamplingIndex, 0));

    TEST_ASSERT_EQUAL_UINT32(0, sensorChannel::extractOversamplingIndex(sensorChannel::compressOversamplingAndPrescalerIndex(0, 0)));
    TEST_ASSERT_EQUAL_UINT32(0, sensorChannel::extractPrescalerIndex(sensorChannel::compressOversamplingAndPrescalerIndex(0, 0)));
    TEST_ASSERT_EQUAL_UINT32(sensorChannel::maxOversamplingIndex, sensorChannel::extractOversamplingIndex(sensorChannel::compressOversamplingAndPrescalerIndex(sensorChannel::maxOversamplingIndex, sensorChannel::maxPrescalerIndex)));
    TEST_ASSERT_EQUAL_UINT32(sensorChannel::maxPrescalerIndex, sensorChannel::extractPrescalerIndex(sensorChannel::compressOversamplingAndPrescalerIndex(sensorChannel::maxOversamplingIndex, sensorChannel::maxPrescalerIndex)));

    // Edge cases
    TEST_ASSERT_EQUAL_UINT8((sensorChannel::maxOversamplingIndex << 4) + sensorChannel::maxPrescalerIndex, sensorChannel::compressOversamplingAndPrescalerIndex(1000, 10000));
    TEST_ASSERT_EQUAL_UINT8(sensorChannel::maxOversamplingIndex, sensorChannel::extractOversamplingIndex(0xFF));
    TEST_ASSERT_EQUAL_UINT8(sensorChannel::maxPrescalerIndex, sensorChannel::extractPrescalerIndex(0xFF));
}

void test_setIndex() {
    sensorChannel testChannel = {0, "", ""};
    testChannel.setIndex(0, 0);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.oversamplingIndex);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.prescalingIndex);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.oversampling);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.prescaling);

    testChannel.setIndex(2, 7);        // Oversampling index 2 corresponds to 4 samples average, prescaler index 7 corresponds to 120 which means 60 minutes
    TEST_ASSERT_EQUAL_UINT32(2, testChannel.oversamplingIndex);
    TEST_ASSERT_EQUAL_UINT32(7, testChannel.prescalingIndex);
    TEST_ASSERT_EQUAL_UINT32(4 - 1, testChannel.oversampling);
    TEST_ASSERT_EQUAL_UINT32(120/4, testChannel.prescaling);

    testChannel.setIndex(sensorChannel::maxOversamplingIndex, sensorChannel::maxPrescalerIndex);
    TEST_ASSERT_EQUAL_UINT32(sensorChannel::maxOversamplingIndex, testChannel.oversamplingIndex);
    TEST_ASSERT_EQUAL_UINT32(sensorChannel::maxPrescalerIndex, testChannel.prescalingIndex);
    TEST_ASSERT_EQUAL_UINT32(sensorChannel::oversamplingLookup[sensorChannel::maxOversamplingIndex] - 1, testChannel.oversampling);
    TEST_ASSERT_EQUAL_UINT32(sensorChannel::prescalerLookup[sensorChannel::maxPrescalerIndex] / sensorChannel::oversamplingLookup[sensorChannel::maxOversamplingIndex], testChannel.prescaling);

    // // Edge cases
    testChannel.setIndex(sensorChannel::maxOversamplingIndex + 1, sensorChannel::maxPrescalerIndex + 1);
    TEST_ASSERT_EQUAL_UINT32(sensorChannel::maxOversamplingIndex, testChannel.oversamplingIndex);
    TEST_ASSERT_EQUAL_UINT32(sensorChannel::maxPrescalerIndex, testChannel.prescalingIndex);
    TEST_ASSERT_EQUAL_UINT32(sensorChannel::oversamplingLookup[sensorChannel::maxOversamplingIndex] - 1, testChannel.oversampling);
    TEST_ASSERT_EQUAL_UINT32(sensorChannel::prescalerLookup[sensorChannel::maxPrescalerIndex] / sensorChannel::oversamplingLookup[sensorChannel::maxOversamplingIndex], testChannel.prescaling);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_set);
    RUN_TEST(test_setAndRestrict);
    RUN_TEST(test_getNextAction);
    RUN_TEST(test_updateCounters);
    RUN_TEST(test_addSample);
    RUN_TEST(test_getOutput);
    RUN_TEST(test_isActive);
    RUN_TEST(test_getNumberOfSamplesToAverage);
    RUN_TEST(test_getMinutesBetweenOutput);
    RUN_TEST(test_compress_and_extract);
    RUN_TEST(test_setIndex);
    UNITY_END();
}
