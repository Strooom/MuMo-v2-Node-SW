#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>
#include <hexascii.hpp>
#include <logging.hpp>
#include <uniqueid.hpp>
#include <i2c.hpp>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

int main(int argc, char **argv) {
    HAL_Init();
    HAL_Delay(3000);
    SystemClock_Config();
    i2c::wakeUp();
    logging::enable(logging::destination::swo);
    uint64_t uid = uniqueId::get();
    char output[17];
    hexAscii::uint64ToHexString(output, uid);
    logging::snprintf("UID: %s\n", output);
}

// 1 : 0080E1150536F0F6
// 2 : 0080E1150537759F
// 3 : 0080E115053772A7
// 4 : 0080E11505360771