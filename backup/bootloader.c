#include "main.h"

// remember to deInitialize the MCU :
// deInit all configured peripherals
// reset clock to boot 4 MHz..


void JumpToBootloader(void)
{
  uint32_t i = 0;
  void (*SysMemBootJump)(void);
  volatile uint32_t BootAddr = 0x1FFF0000;

  __disable_irq();
  SysTick->CTRL = 0;
  HAL_RCC_DeInit();
  for (i = 0; i < 5; i++)
  {
    NVIC->ICER[i] = 0xFFFFFFFF;
    NVIC->ICPR[i] = 0xFFFFFFFF;
  }
  __enable_irq();

  SysMemBootJump = (void (*)(void))(*((uint32_t *)((BootAddr + 4))));

  /* Set the main stack pointer to the boot loader stack */
  __set_MSP(*(uint32_t *)BootAddr);

  /* Call the function to jump to boot loader location */
  SysMemBootJump();

  /* Jump is done successfully */
  while (1)
  {
    /* Code should never reach this loop */
  }
}
