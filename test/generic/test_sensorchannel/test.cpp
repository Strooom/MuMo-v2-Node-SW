#include <unity.h>
#include <power.hpp>
#include <sensorchannel.hpp>

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

extern bool mockUsbPower;

void test_initalize() {
    sensorChannel testChannel = {0, "", ""};
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.oversamplingLowPower);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.prescalerLowPower);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.oversamplingHighPower);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.prescalerHighPower);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.prescaleCounter);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.oversamplingCounter);

    for (uint32_t index = 0; index < sensorChannel::maxOversampling; index++) {
        TEST_ASSERT_EQUAL_FLOAT(0.0F, testChannel.samples[index]);
    }
}

void test_set() {
    sensorChannel testChannel = {0, "", ""};
    mockUsbPower = false;
    testChannel.set(1, 2, 3, 4, 5.0F);
    TEST_ASSERT_EQUAL_UINT32(1, testChannel.oversamplingLowPower);
    TEST_ASSERT_EQUAL_UINT32(2, testChannel.prescalerLowPower);
    TEST_ASSERT_EQUAL_UINT32(3, testChannel.oversamplingHighPower);
    TEST_ASSERT_EQUAL_UINT32(4, testChannel.prescalerHighPower);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.oversamplingCounter);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.prescaleCounter);

    mockUsbPower = true;
    testChannel.set(1, 2, 3, 4, 5.0F);
    TEST_ASSERT_EQUAL_UINT32(1, testChannel.oversamplingLowPower);
    TEST_ASSERT_EQUAL_UINT32(2, testChannel.prescalerLowPower);
    TEST_ASSERT_EQUAL_UINT32(3, testChannel.oversamplingHighPower);
    TEST_ASSERT_EQUAL_UINT32(4, testChannel.prescalerHighPower);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.oversamplingCounter);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.prescaleCounter);
}

void test_setAndRestrict() {
    sensorChannel testChannel = {0, "", ""};
    mockUsbPower = false;
    testChannel.set(testChannel.maxOversampling + 1, testChannel.maxPrescaler + 1, testChannel.maxOversampling + 1, testChannel.maxPrescaler + 1, 5.0F);
    TEST_ASSERT_EQUAL_UINT32(testChannel.maxOversampling, testChannel.oversamplingLowPower);
    TEST_ASSERT_EQUAL_UINT32(testChannel.maxPrescaler, testChannel.prescalerLowPower);
    TEST_ASSERT_EQUAL_UINT32(testChannel.maxOversampling, testChannel.oversamplingHighPower);
    TEST_ASSERT_EQUAL_UINT32(testChannel.maxPrescaler, testChannel.prescalerHighPower);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.oversamplingCounter);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.prescaleCounter);

    mockUsbPower = true;
    testChannel.set(testChannel.maxOversampling + 1, testChannel.maxPrescaler + 1, testChannel.maxOversampling + 1, testChannel.maxPrescaler + 1, 5.0F);
    TEST_ASSERT_EQUAL_UINT32(testChannel.maxOversampling, testChannel.oversamplingLowPower);
    TEST_ASSERT_EQUAL_UINT32(testChannel.maxPrescaler, testChannel.prescalerLowPower);
    TEST_ASSERT_EQUAL_UINT32(testChannel.maxOversampling, testChannel.oversamplingHighPower);
    TEST_ASSERT_EQUAL_UINT32(testChannel.maxPrescaler, testChannel.prescalerHighPower);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.oversamplingCounter);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.prescaleCounter);
}

void test_getCurrentPrescaler() {
    sensorChannel testChannel = {0, "", ""};
    testChannel.set(1, 2, 3, 4, 5.0F);

    mockUsbPower = false;
    TEST_ASSERT_EQUAL_UINT32(2, testChannel.getCurrentPrescaler());
    mockUsbPower = true;
    TEST_ASSERT_EQUAL_UINT32(4, testChannel.getCurrentPrescaler());
}

void test_getCurrentOversampling() {
    sensorChannel testChannel = {0, "", ""};
    testChannel.set(1, 2, 3, 4, 5.0F);

    mockUsbPower = false;
    TEST_ASSERT_EQUAL_UINT32(1, testChannel.getCurrentOversampling());
    mockUsbPower = true;
    TEST_ASSERT_EQUAL_UINT32(3, testChannel.getCurrentOversampling());
}

void test_getNextAction() {
    sensorChannel testChannel = {0, "", ""};
    TEST_ASSERT_EQUAL(sensorChannel::action::none, testChannel.getNextAction());

    testChannel.set(2, 2, 0, 0, 5.0F);
    mockUsbPower = false;

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
    mockUsbPower = false;
    sensorChannel testChannel = {0, "", ""};
    testChannel.set(1, 4, 0, 0, 5.0F);        // = oversampling = 1 -> average 2 samples into a measurement, prescaling = 4 -> take a sample every 4th RTC tick

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
    mockUsbPower = false;
    sensorChannel testChannel = {0, "", ""};        // = oversampling = 3 + 1 = 4, prescaling = 1 (= no prescaling)
    testChannel.set(3, 1, 0, 0, 5.0F);
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
    testChannel.set(3, 1, 0, 0, 5.0F);
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

void test_sensor_transition_high_low_power() {
    mockUsbPower = true;
    sensorChannel testChannel = {0, "", ""};
    testChannel.set(3, 4, 7, 8, 5.0F);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.oversamplingCounter);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel.prescaleCounter);
    testChannel.updateCounters();
    TEST_ASSERT_EQUAL_UINT32(7, testChannel.oversamplingCounter);
    TEST_ASSERT_EQUAL_UINT32(7, testChannel.prescaleCounter);
    testChannel.updateCounters();
    TEST_ASSERT_EQUAL_UINT32(7, testChannel.oversamplingCounter);
    TEST_ASSERT_EQUAL_UINT32(6, testChannel.prescaleCounter);

    mockUsbPower = false;

    testChannel.updateCounters();
    TEST_ASSERT_EQUAL_UINT32(3, testChannel.oversamplingCounter);
    TEST_ASSERT_EQUAL_UINT32(3, testChannel.prescaleCounter);
    testChannel.updateCounters();
    TEST_ASSERT_EQUAL_UINT32(3, testChannel.oversamplingCounter);
    TEST_ASSERT_EQUAL_UINT32(2, testChannel.prescaleCounter);

    mockUsbPower = true;
    testChannel.updateCounters();
    TEST_ASSERT_EQUAL_UINT32(3, testChannel.oversamplingCounter);
    TEST_ASSERT_EQUAL_UINT32(1, testChannel.prescaleCounter);
}

void test_average() {
    sensorChannel testChannel = {0, "", ""};
    testChannel.set(4, 360, 8, 15, 5.0F);

    for (auto index = 0; index < sensorChannel::maxOversampling + 1; index++) {
        testChannel.samples[index] = static_cast<float>(index);
    }

    TEST_ASSERT_EQUAL_FLOAT(0.0F, testChannel.average(1));
    TEST_ASSERT_EQUAL_FLOAT(0.5F, testChannel.average(2));
    TEST_ASSERT_EQUAL_FLOAT(1.5F, testChannel.average(4));

    // edge cases..
    TEST_ASSERT_EQUAL_FLOAT(0.0F, testChannel.average(0));
    TEST_ASSERT_EQUAL_FLOAT(7.5F, testChannel.average(1000));        // all 16 samples amount to 120
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initalize);
    RUN_TEST(test_set);
    RUN_TEST(test_setAndRestrict);
    RUN_TEST(test_getCurrentPrescaler);
    RUN_TEST(test_getCurrentOversampling);
    RUN_TEST(test_getNextAction);
    RUN_TEST(test_updateCounters);
    RUN_TEST(test_addSample);
    RUN_TEST(test_getOutput);
    RUN_TEST(test_sensor_transition_high_low_power);
    RUN_TEST(test_average);
    UNITY_END();
}
