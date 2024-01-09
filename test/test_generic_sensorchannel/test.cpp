#include <unity.h>
#include <power.hpp>
#include <sensorchannel.hpp>

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_initalize() {
    sensorChannel testChannel1{sensorChannelType::batteryChargeLevel};
    TEST_ASSERT_EQUAL(sensorChannelType::batteryChargeLevel, testChannel1.type);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel1.oversamplingLowPower);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel1.prescalerLowPower);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel1.oversamplingHighPower);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel1.prescalerHighPower);

    TEST_ASSERT_EQUAL_UINT32(0, testChannel1.prescaleCounter);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel1.oversamplingCounter);

    for (uint32_t index = 0; index < sensorChannel::maxOversampling; index++) {
        TEST_ASSERT_EQUAL_FLOAT(0.0F, testChannel1.samples[index]);
    }

    sensorChannel testChannel2{sensorChannelType::batteryVoltage};
    testChannel2.set(10000, 10000, 10000, 10000);
    TEST_ASSERT_EQUAL(sensorChannelType::batteryVoltage, testChannel2.type);
    TEST_ASSERT_EQUAL_UINT32(sensorChannel::maxOversampling, testChannel2.oversamplingLowPower);
    TEST_ASSERT_EQUAL_UINT32(sensorChannel::maxPrescaler, testChannel2.prescalerLowPower);
    TEST_ASSERT_EQUAL_UINT32(sensorChannel::maxOversampling, testChannel2.oversamplingHighPower);
    TEST_ASSERT_EQUAL_UINT32(sensorChannel::maxPrescaler, testChannel2.prescalerHighPower);
}

void test_set() {
    sensorChannel testChannel1{sensorChannelType::batteryChargeLevel};
    testChannel1.set(4, 360, 8, 15);
    TEST_ASSERT_EQUAL_UINT32(4, testChannel1.oversamplingLowPower);
    TEST_ASSERT_EQUAL_UINT32(360, testChannel1.prescalerLowPower);
    TEST_ASSERT_EQUAL_UINT32(8, testChannel1.oversamplingHighPower);
    TEST_ASSERT_EQUAL_UINT32(15, testChannel1.prescalerHighPower);
}

void test_getCurrentPrescaler() {
    sensorChannel testChannel{sensorChannelType::none};
    testChannel.set(1, 2, 3, 4);

    power::mockUsbPower = false;
    TEST_ASSERT_EQUAL_UINT32(2, testChannel.getCurrentPrescaler());
    power::mockUsbPower = true;
    TEST_ASSERT_EQUAL_UINT32(4, testChannel.getCurrentPrescaler());
}

void test_getCurrentOversampling() {
    sensorChannel testChannel{sensorChannelType::none};
    testChannel.set(1, 2, 3, 4);

    power::mockUsbPower = false;
    TEST_ASSERT_EQUAL_UINT32(1, testChannel.getCurrentOversampling());
    power::mockUsbPower = true;
    TEST_ASSERT_EQUAL_UINT32(3, testChannel.getCurrentOversampling());
}

void test_getNextAction() {
    sensorChannel testChannel1{sensorChannelType::none};
    TEST_ASSERT_EQUAL(sensorChannel::action::none, testChannel1.getNextAction());

    sensorChannel testChannel2{sensorChannelType::none};
    testChannel2.set(2, 2, 0, 0);
    power::mockUsbPower = false;

    testChannel2.prescaleCounter     = 1;
    testChannel2.oversamplingCounter = 0;
    TEST_ASSERT_EQUAL(sensorChannel::action::prescale, testChannel2.getNextAction());

    testChannel2.prescaleCounter     = 1;
    testChannel2.oversamplingCounter = 1;
    TEST_ASSERT_EQUAL(sensorChannel::action::prescale, testChannel2.getNextAction());

    testChannel2.prescaleCounter     = 0;
    testChannel2.oversamplingCounter = 1;
    TEST_ASSERT_EQUAL(sensorChannel::action::sample, testChannel2.getNextAction());

    testChannel2.prescaleCounter     = 0;
    testChannel2.oversamplingCounter = 0;
    TEST_ASSERT_EQUAL(sensorChannel::action::sampleAndOutput, testChannel2.getNextAction());
}

void test_adjustCounters() {
    sensorChannel testChannel1{sensorChannelType::none};
    testChannel1.prescaleCounter     = 1;
    testChannel1.oversamplingCounter = 1;
    testChannel1.adjustCounters();
    TEST_ASSERT_EQUAL_UINT32(0, testChannel1.prescaleCounter);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel1.oversamplingCounter);

    sensorChannel testChannel2{sensorChannelType::none};        // = oversampling = 3, prescaling = 2
    testChannel2.set(2, 2, 0, 0);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel2.prescaleCounter);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel2.oversamplingCounter);
    TEST_ASSERT_EQUAL(sensorChannel::action::sampleAndOutput, testChannel2.getNextAction());
    testChannel2.adjustCounters();
    TEST_ASSERT_EQUAL_UINT32(1, testChannel2.prescaleCounter);
    TEST_ASSERT_EQUAL_UINT32(2, testChannel2.oversamplingCounter);
    TEST_ASSERT_EQUAL(sensorChannel::action::prescale, testChannel2.getNextAction());
    testChannel2.adjustCounters();
    TEST_ASSERT_EQUAL_UINT32(0, testChannel2.prescaleCounter);
    TEST_ASSERT_EQUAL_UINT32(2, testChannel2.oversamplingCounter);
    TEST_ASSERT_EQUAL(sensorChannel::action::sample, testChannel2.getNextAction());
    testChannel2.adjustCounters();
    TEST_ASSERT_EQUAL_UINT32(1, testChannel2.prescaleCounter);
    TEST_ASSERT_EQUAL_UINT32(1, testChannel2.oversamplingCounter);
    TEST_ASSERT_EQUAL(sensorChannel::action::prescale, testChannel2.getNextAction());
    testChannel2.adjustCounters();
    TEST_ASSERT_EQUAL_UINT32(0, testChannel2.prescaleCounter);
    TEST_ASSERT_EQUAL_UINT32(1, testChannel2.oversamplingCounter);
    TEST_ASSERT_EQUAL(sensorChannel::action::sample, testChannel2.getNextAction());
    testChannel2.adjustCounters();
    TEST_ASSERT_EQUAL_UINT32(1, testChannel2.prescaleCounter);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel2.oversamplingCounter);
    TEST_ASSERT_EQUAL(sensorChannel::action::prescale, testChannel2.getNextAction());
    testChannel2.adjustCounters();
    TEST_ASSERT_EQUAL_UINT32(0, testChannel2.prescaleCounter);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel2.oversamplingCounter);
    TEST_ASSERT_EQUAL(sensorChannel::action::sampleAndOutput, testChannel2.getNextAction());
}

void test_addSample() {
    sensorChannel testChannel{sensorChannelType::none};        // = oversampling = 3 + 1 = 4, prescaling = 1 (= no prescaling)
    testChannel.set(3, 1, 0, 0);

    testChannel.adjustCounters();
    testChannel.addSample(13.3F);
    testChannel.adjustCounters();
    testChannel.addSample(12.2F);
    testChannel.adjustCounters();
    testChannel.addSample(11.1F);
    testChannel.adjustCounters();
    testChannel.addSample(10.0F);

    TEST_ASSERT_EQUAL_FLOAT(13.3F, testChannel.samples[3]);
    TEST_ASSERT_EQUAL_FLOAT(12.2F, testChannel.samples[2]);
    TEST_ASSERT_EQUAL_FLOAT(11.1F, testChannel.samples[1]);
    TEST_ASSERT_EQUAL_FLOAT(10.0F, testChannel.samples[0]);

    TEST_ASSERT_EQUAL_FLOAT(0.0F, testChannel.samples[4]);

    testChannel.adjustCounters();
    testChannel.addSample(13.3F);
    testChannel.adjustCounters();
    testChannel.addSample(12.2F);
    testChannel.adjustCounters();
    testChannel.addSample(11.1F);
    testChannel.adjustCounters();
    testChannel.addSample(10.0F);

    TEST_ASSERT_EQUAL_FLOAT(13.3F, testChannel.samples[3]);
    TEST_ASSERT_EQUAL_FLOAT(12.2F, testChannel.samples[2]);
    TEST_ASSERT_EQUAL_FLOAT(11.1F, testChannel.samples[1]);
    TEST_ASSERT_EQUAL_FLOAT(10.0F, testChannel.samples[0]);
}

void test_getOutput() {
    sensorChannel testChannel{sensorChannelType::none};        // = oversampling = 3 + 1 = 4, prescaling = 1 (= no prescaling)
    testChannel.set(3, 1, 0, 0);
    testChannel.adjustCounters();
    testChannel.addSample(1.0F);
    testChannel.adjustCounters();
    testChannel.addSample(2.0F);
    testChannel.adjustCounters();
    testChannel.addSample(3.0F);
    testChannel.adjustCounters();
    testChannel.addSample(4.0F);

    TEST_ASSERT_EQUAL_FLOAT(2.5F, testChannel.getOutput());

    testChannel.adjustCounters();
    testChannel.addSample(10.0F);
    testChannel.adjustCounters();
    testChannel.addSample(20.0F);
    testChannel.adjustCounters();
    testChannel.addSample(30.0F);
    testChannel.adjustCounters();
    testChannel.addSample(40.0F);

    TEST_ASSERT_EQUAL_FLOAT(25.0F, testChannel.getOutput());
}

void test_sensor_transition_high_low_power() {
    sensorChannel testChannel1{sensorChannelType::none};
        testChannel1.set(3, 1, 9, 10);

    power::mockUsbPower = true;
    testChannel1.adjustCounters();
    TEST_ASSERT_EQUAL_UINT32(9, testChannel1.oversamplingCounter);
    TEST_ASSERT_EQUAL_UINT32(9, testChannel1.prescaleCounter);

    power::mockUsbPower = false;
    testChannel1.adjustCounters();
    TEST_ASSERT_EQUAL_UINT32(3, testChannel1.oversamplingCounter);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel1.prescaleCounter);

    sensorChannel testChannel2{sensorChannelType::none};
        testChannel2.set(3, 1, 9, 10);
    power::mockUsbPower = false;
    testChannel2.adjustCounters();
    TEST_ASSERT_EQUAL_UINT32(3, testChannel2.oversamplingCounter);
    TEST_ASSERT_EQUAL_UINT32(0, testChannel2.prescaleCounter);

    power::mockUsbPower = true;
    testChannel2.adjustCounters();
    TEST_ASSERT_EQUAL_UINT32(2, testChannel2.oversamplingCounter);
    TEST_ASSERT_EQUAL_UINT32(9, testChannel2.prescaleCounter);
}

void test_average() {
    sensorChannel testChannel{sensorChannelType::batteryChargeLevel};
            testChannel.set(4, 360, 8, 15);

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

void test_dummy() {
    (void)toString(sensorChannelType::batteryChargeLevel);
    (void)toString(sensorChannelType::batteryVoltage);
    (void)toString(sensorChannelType::BME680BarometricPressure);
    (void)toString(sensorChannelType::BME680RelativeHumidity);
    (void)toString(sensorChannelType::BME680Temperature);
    (void)toString(sensorChannelType::events);
    (void)toString(sensorChannelType::none);
    (void)toString(sensorChannelType::status);
    (void)toString(sensorChannelType::TSL25911Infrared);
    (void)toString(sensorChannelType::TSL25911VisibleLight);
    TEST_IGNORE_MESSAGE("For testCoverage only");
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initalize);
    RUN_TEST(test_set);
    RUN_TEST(test_getCurrentPrescaler);
    RUN_TEST(test_getCurrentOversampling);
    RUN_TEST(test_getNextAction);
    RUN_TEST(test_adjustCounters);
    RUN_TEST(test_addSample);
    RUN_TEST(test_getOutput);
    RUN_TEST(test_sensor_transition_high_low_power);
    RUN_TEST(test_average);
    RUN_TEST(test_dummy);
    UNITY_END();
}
