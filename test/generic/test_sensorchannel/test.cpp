#include <unity.h>
#include <sensorchannel.hpp>

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_initalize() {
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


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initalize);
    RUN_TEST(test_set);
    RUN_TEST(test_setAndRestrict);
    RUN_TEST(test_getNextAction);
    RUN_TEST(test_updateCounters);
    RUN_TEST(test_addSample);
    RUN_TEST(test_getOutput);
    UNITY_END();
}
