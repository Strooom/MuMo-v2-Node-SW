https://community.st.com/s/article/how-to-jump-to-system-bootloader-from-application-code-on-stm32-microcontrollers

* 16 Kbytes starting from address 0x1FFF0000, contain the bootloader firmware

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* Set the enumeration of the STM32 Microcontrollers Series*/
enum{C0, F030x8, F030xC, F03xx, F05, F07, F09, F10xx, F105, F107, F10XL, F2, F3, F4, F7, G0, G4, H503, H563, H573, H7x, H7A, H7B, L0, L1, L4, L5, WBA, WBX, WL, U5};

#define MCU			WL		//Define here the MCU being used


void JumpToBootloader (void)
{
uint32_t i=0;
void (*SysMemBootJump)(void); // This declares a pointer to a function (with no parameters and no return value)

	

	__disable_irq(); // Disable all interrupts
	SysTick->CTRL = 0; // Disable Systick timer
	HAL_RCC_DeInit(); // 	Set the clock to the default state

// Note : more stuff which was initialized needs to be reset..

	// Clear Interrupt Enable Register & Interrupt Pending Register
	for (i=0;i<5;i++)
	{
		NVIC->ICER[i]=0xFFFFFFFF;
		NVIC->ICPR[i]=0xFFFFFFFF;
	}

	
	__enable_irq(); // Re-enable all interrupts

	SysMemBootJump = (void (*)(void)) (*((uint32_t *) (0x1FFF0004))); // Set up the jump to boot loader address + 4
	__set_MSP(*(uint32_t *)BootAddr[MCU]); 	// Set the main stack pointer to the boot loader stack
	SysMemBootJump(); // 	Call the function to jump to boot loader location


	while (1)	{	}
}


Checking if this works : hit pause and the programCounter address should be in the 16K range after the startAddress