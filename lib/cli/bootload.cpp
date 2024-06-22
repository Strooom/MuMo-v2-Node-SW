#ifndef generic
#include <main.h>
extern void HAL_UART_DeInit();
#endif
#include <cstdint>

// Need all kinds of HAL functions to reset the MCU to a clean state

static constexpr uint8_t bootLoaderMagicValue{0x7F};

void jumpToBootLoader() {
#ifndef generic
    HAL_UART_DeInit(&huart2);        // TODO : maybe need to reset some other stuff as well, to bring the MCU back to a clean state as after reset

    uint32_t i = 0;
    void (*SysMemBootJump)(void);
    volatile uint32_t BootAddr = 0x1FFF0000;

    __disable_irq();
    SysTick->CTRL = 0;
    HAL_RCC_DeInit();
    for (i = 0; i < 5; i++) {
        NVIC->ICER[i] = 0xFFFFFFFF;
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }
    __enable_irq();

    SysMemBootJump = (void (*)(void))(*((uint32_t *)((BootAddr + 4))));

    __set_MSP(*(uint32_t *)BootAddr);        // Set the main stack pointer to the boot loader stack

    SysMemBootJump();
#endif
    while (1) {
    }
}
