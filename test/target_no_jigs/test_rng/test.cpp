#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

void test_randomNumberGenerator() {
    static constexpr uint32_t nmbrofTests{20};
    uint32_t result{0};

    for (uint32_t testIndex = 0; testIndex < nmbrofTests; testIndex++) {
        HAL_RNG_GenerateRandomNumber(&hrng, &result);
        TEST_ASSERT_NOT_EQUAL(0, result);
    }
}

int main(int argc, char **argv) {
    HAL_Init();
    HAL_Delay(2000);        // required for testing framework to connect
    SystemClock_Config();
    HAL_RNG_MspInit(&hrng);
    MX_RNG_Init();

    UNITY_BEGIN();
    RUN_TEST(test_randomNumberGenerator);
    UNITY_END();
}
