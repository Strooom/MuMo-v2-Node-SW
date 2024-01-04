#include <unity.h>
#include "power.h"
#include "channelcollection.h"

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_initalize() {
    TEST_ASSERT_EQUAL_UINT32(868'100'000U, loRaChannelCollection::txRxChannels[0].frequency);
    TEST_ASSERT_EQUAL_UINT32(868'300'000U, loRaChannelCollection::txRxChannels[1].frequency);
    TEST_ASSERT_EQUAL_UINT32(868'500'000U, loRaChannelCollection::txRxChannels[2].frequency);

    for (auto index = 3U; index < static_cast<uint32_t>(loRaChannelCollection::maxNmbrChannels); index++) {
        TEST_ASSERT_EQUAL_UINT32(0U, loRaChannelCollection::txRxChannels[index].frequency);
    }
    TEST_ASSERT_EQUAL_UINT32(869'525'000U, loRaChannelCollection::rx2Channel.frequency);

    TEST_ASSERT_EQUAL_UINT32(0U, loRaChannelCollection::currentChannelIndex);
}

void test_channel_selection() {
    loRaChannelCollection::selectNextActiveChannelIndex();
    TEST_ASSERT_EQUAL_UINT32(1U, loRaChannelCollection::currentChannelIndex);
    loRaChannelCollection::selectNextActiveChannelIndex();
    TEST_ASSERT_EQUAL_UINT32(2U, loRaChannelCollection::currentChannelIndex);
    loRaChannelCollection::selectNextActiveChannelIndex();
    TEST_ASSERT_EQUAL_UINT32(0U, loRaChannelCollection::currentChannelIndex);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initalize);
    RUN_TEST(test_channel_selection);
    UNITY_END();
}
