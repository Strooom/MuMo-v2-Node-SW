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

void test_calculateOversampling() {
    TEST_ASSERT_EQUAL_UINT32(0, sensorChannel::calculateOversampling(0));        // numberOfSamplesToAverage cannot be zero

    TEST_ASSERT_EQUAL_UINT32(0, sensorChannel::calculateOversampling(1));
    TEST_ASSERT_EQUAL_UINT32(1, sensorChannel::calculateOversampling(2));
    TEST_ASSERT_EQUAL_UINT32(2, sensorChannel::calculateOversampling(3));
    TEST_ASSERT_EQUAL_UINT32(3, sensorChannel::calculateOversampling(4));
    TEST_ASSERT_EQUAL_UINT32(4, sensorChannel::calculateOversampling(5));
    TEST_ASSERT_EQUAL_UINT32(5, sensorChannel::calculateOversampling(6));
    TEST_ASSERT_EQUAL_UINT32(6, sensorChannel::calculateOversampling(7));
    TEST_ASSERT_EQUAL_UINT32(7, sensorChannel::calculateOversampling(8));

    TEST_ASSERT_EQUAL_UINT32(7, sensorChannel::calculateOversampling(9));        // numberOfSamplesToAverage cannot be larger than maxOversampling + 1
}

void test_calculatePrescaler() {
    TEST_ASSERT_EQUAL_UINT32(2, sensorChannel::calculatePrescaler(1, 1));          // no oversampling, 1 output every 1 minute = 2 RTC ticks
    TEST_ASSERT_EQUAL_UINT32(20, sensorChannel::calculatePrescaler(10, 1));        // no oversampling, 1 output every 10 minutes = 20 RTC ticks
    TEST_ASSERT_EQUAL_UINT32(30, sensorChannel::calculatePrescaler(60, 4));        // 4 samples to average, 1 output every 60 minutes = 30 RTC ticks
    TEST_ASSERT_EQUAL_UINT32(4, sensorChannel::calculatePrescaler(16, 8));         // correct usage for edge case further below

    // edge cases / error cases
    TEST_ASSERT_EQUAL_UINT32(2, sensorChannel::calculatePrescaler(0, 1));          // minutes between cannot be zero
    TEST_ASSERT_EQUAL_UINT32(4, sensorChannel::calculatePrescaler(16, 16));        // numberOfSamplesToAverage cannot be larger than maxOversampling + 1, will be limited and then used for calculation
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
    RUN_TEST(test_calculateOversampling);
    RUN_TEST(test_calculatePrescaler);
    RUN_TEST(test_getNumberOfSamplesToAverage);
    RUN_TEST(test_getMinutesBetweenOutput);
    UNITY_END();
}
