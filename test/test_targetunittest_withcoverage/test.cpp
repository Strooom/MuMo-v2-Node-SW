#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>
#include <stdio.h>
#include <gpio.hpp>
#include <sht40.hpp>
#include <unity.h>
#include <gcov.h>
#include <gcov_support.h>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

#ifdef __cplusplus
extern "C" {
#endif
void initialise_monitor_handles(void);
#ifdef __cplusplus
}
#endif /* extern "C" */

void setUp(void) {}
void tearDown(void) {}

void test_isPresent() {
    TEST_ASSERT_TRUE(sht40::isPresent());
}

int main(int argc, char **argv) {
    // gcov_init();
    void (**p)(void);
    extern uint32_t __init_array_start, __init_array_end; /* linker defined symbols, array of function pointers */
    uint32_t beg = (uint32_t)&__init_array_start;
    uint32_t end = (uint32_t)&__init_array_end;

    while (beg < end) {
        p = (void (**)(void))beg; /* get function pointer */
        (*p)();                   /* call constructor */
        beg += sizeof(p);         /* next pointer */
    }

    initialise_monitor_handles();
    HAL_Init();
    HAL_Delay(2000);        // required for testing framework to connect
    SystemClock_Config();

    MX_I2C2_Init();
    gpio::enableGpio(gpio::group::vddEnable);
    HAL_GPIO_WritePin(GPIOA, vddEnable_Pin, GPIO_PIN_RESET);

    gpio::enableGpio(gpio::group::i2cEeprom);

    UNITY_BEGIN();
    RUN_TEST(test_isPresent);
    UNITY_END();
    //gcov_write();
      __gcov_flush();
}
