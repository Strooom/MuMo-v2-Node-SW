#include <unity.h>
#include "main.h"

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_encrypt() {
    TEST_IGNORE_MESSAGE("Implement me!");
}

int main(int argc, char **argv) {
    HAL_Init();             // initialize the HAL library
    HAL_Delay(2000);        // service delay, could be required wrt resetting the board
    UNITY_BEGIN();
    RUN_TEST(test_encrypt);
    UNITY_END();
}

void SysTick_Handler(void) {
    HAL_IncTick();
}