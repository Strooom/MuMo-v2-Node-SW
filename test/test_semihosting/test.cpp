#include <unity.h>
#include "main.h"
#include <cube.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <stm32wlxx_hal_msp.c>
#include <stm32wlxx_it.cpp>
#include <realtimeclock.hpp>
#include <logging.hpp>
#include <stdio.h>

circularBuffer<applicationEvent, 16U> applicationEventBuffer;

#ifdef __cplusplus
extern "C" {
#endif
void initialise_monitor_handles(void);
#ifdef __cplusplus
}
#endif /* extern "C" */

int McuSemihost_Op_SYS_TIME         = 0x11;

static inline int __attribute__ ((always_inline)) McuSemihost_HostRequest(int reason, void *arg) {
  int value;
  __asm volatile (
      "mov r0, %[rsn] \n" /* place semihost operation code into R0 */
      "mov r1, %[arg] \n" /* R1 points to the argument array */
      "bkpt 0xAB      \n" /* call debugger */
      "mov %[val], r0 \n" /* debugger has stored result code in R0 */

      : [val] "=r" (value) /* outputs */
      : [rsn] "r" (reason), [arg] "r" (arg) /* inputs */
      : "r0", "r1", "r2", "r3", "ip", "lr", "memory", "cc" /* clobber */
  );
  return value; /* return result code, stored in R0 */
}


void setUp(void) {        // before each test
}
void tearDown(void) {        // after each test
}

int main(int argc, char **argv) {
    initialise_monitor_handles();
    HAL_Init();
    HAL_Delay(2000);
    SystemClock_Config();
    MX_RTC_Init();
    MX_USART1_UART_Init();

    logging::enable(logging::destination::uart1);
    logging::snprintf("Start\r\n");

    
    time_t unixTime;
    unixTime = McuSemihost_HostRequest(McuSemihost_Op_SYS_TIME, NULL);

    struct tm *brokendownTime = gmtime(&unixTime);

    //  char buffer[50]{0};
    //  sprintf(buffer, "%s", ctime(&unixTime));

    logging::snprintf("Time : %s\r\n", asctime(brokendownTime));

    while (1) {
        __NOP();
    }

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
