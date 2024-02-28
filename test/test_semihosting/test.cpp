#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>

#include <stdio.h>

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

int main(int argc, char **argv) {
    initialise_monitor_handles();
    HAL_Init();
    HAL_Delay(2000);
    SystemClock_Config();

    // FILE *file;
    // file = fopen("demo.txt", "w");
    // if (file != NULL) {
    //     fputs("hello world with file I/O\r\n", file);
    //     (void)fwrite("hello\r\n", sizeof("hello\r\n") - 1, 1, file);
    //     fclose(file);
    // }
    // file = fopen("demo2.txt", "w");
    // if (file != NULL) {
    //     fputs("hello world with file I/O\r\n", file);
    //     (void)fwrite("hello\r\n", sizeof("hello\r\n") - 1, 1, file);
    //     fclose(file);
    // }
}
