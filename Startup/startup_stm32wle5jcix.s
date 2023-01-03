/**
 ******************************************************************************
 * @file      startup_stm32wle5jcix.s
 * @author    Auto-generated by STM32CubeIDE
 * @brief     STM32WLE5JCIx device vector table for GCC toolchain.
 *            This module performs:
 *                - Set the initial SP
 *                - Set the initial PC == Reset_Handler,
 *                - Set the vector table entries with the exceptions ISR address
 *                - Branches to main in the C library (which eventually
 *                  calls main()).
 ******************************************************************************
 * @attention
 *
 * Copyright (c) ${year} STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.global g_pfnVectors
.global Default_Handler

/* start address for the initialization values of the .data section.
defined in linker script */
.word _sidata
/* start address for the .data section. defined in linker script */
.word _sdata
/* end address for the .data section. defined in linker script */
.word _edata
/* start address for the .bss section. defined in linker script */
.word _sbss
/* end address for the .bss section. defined in linker script */
.word _ebss

/**
 * @brief  This is the code that gets called when the processor first
 *          starts execution following a reset event. Only the absolutely
 *          necessary set is performed, after which the application
 *          supplied main() routine is called.
 * @param  None
 * @retval : None
*/

  .section .text.Reset_Handler
  .weak Reset_Handler
  .type Reset_Handler, %function
Reset_Handler:
  ldr   r0, =_estack
  mov   sp, r0          /* set stack pointer */
/* Call the clock system initialization function.*/
  bl  SystemInit

/* Copy the data segment initializers from flash to SRAM */
  ldr r0, =_sdata
  ldr r1, =_edata
  ldr r2, =_sidata
  movs r3, #0
  b LoopCopyDataInit

CopyDataInit:
  ldr r4, [r2, r3]
  str r4, [r0, r3]
  adds r3, r3, #4

LoopCopyDataInit:
  adds r4, r0, r3
  cmp r4, r1
  bcc CopyDataInit

/* Zero fill the bss segment. */
  ldr r2, =_sbss
  ldr r4, =_ebss
  movs r3, #0
  b LoopFillZerobss

FillZerobss:
  str  r3, [r2]
  adds r2, r2, #4

LoopFillZerobss:
  cmp r2, r4
  bcc FillZerobss

/* Call static constructors */
  bl __libc_init_array
/* Call the application's entry point.*/
  bl main

LoopForever:
    b LoopForever

  .size Reset_Handler, .-Reset_Handler

/**
 * @brief  This is the code that gets called when the processor receives an
 *         unexpected interrupt.  This simply enters an infinite loop, preserving
 *         the system state for examination by a debugger.
 *
 * @param  None
 * @retval : None
*/
  .section .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
  b Infinite_Loop
  .size Default_Handler, .-Default_Handler

/******************************************************************************
*
* The STM32WLE5JCIx vector table.  Note that the proper constructs
* must be placed on this to ensure that it ends up at physical address
* 0x0000.0000.
*
******************************************************************************/
  .section .isr_vector,"a",%progbits
  .type g_pfnVectors, %object
  .size g_pfnVectors, .-g_pfnVectors

g_pfnVectors:
  .word _estack
  .word Reset_Handler
  .word NMI_Handler
  .word HardFault_Handler
  .word	MemManage_Handler
  .word	BusFault_Handler
  .word	UsageFault_Handler
  .word	0
  .word	0
  .word	0
  .word	0
  .word	SVC_Handler
  .word	DebugMon_Handler
  .word	0
  .word	PendSV_Handler
  .word	SysTick_Handler
  .word	WWDG_IRQHandler                        			/* Window watchdog early wakeup interrupt                                    */
  .word	PVD_PVM_3_IRQHandler                   			/* PVD through EXTI[16], PVM[3] through EXTI[34]                             */
  .word	TAMP_RTCSTAMP_LSECSS_RTCSSRU_IRQHandler			/* Tamper, TimeStamp, LSECSS,RTC_SSRU interrupt                              */
  .word	RTC_WKUP_IRQHandler                    			/* RTC wakeup interrupt                                                      */
  .word	FLASH_IRQHandler                       			/* Flash memory global interrupt and Flash memory ECC single error interrupt */
  .word	RCC_IRQHandler                         			/* RCC global interrupt                                                      */
  .word	EXTI0_IRQHandler                       			/* EXTI line 0 interrupt through EXTI                                        */
  .word	EXTI1_IRQHandler                       			/* EXTI line 1 interrupt through EXTI                                        */
  .word	EXTI2_IRQHandler                       			/* EXTI line 2 interrupt through EXTI                                        */
  .word	EXTI3_IRQHandler                       			/* EXTI line 3 interrupt through EXTI                                        */
  .word	EXTI4_IRQHandler                       			/* EXTI line 4 interrupt through EXTI                                        */
  .word	DMA1_CH1_IRQHandler                    			/* DMA1 channel 1 non-secure interrupt                                       */
  .word	DMA1_CH2_IRQHandler                    			/* DMA1 channel 2 non-secure interrupt                                       */
  .word	DMA1_CH3_IRQHandler                    			/* DMA1 channel 3 non-secure interrupt                                       */
  .word	DMA1_CH4_IRQHandler                    			/* DMA1 channel 4 non-secure interrupt                                       */
  .word	DMA1_CH5_IRQHandler                    			/* DMA1 channel 5 non-secure interrupt                                       */
  .word	DMA1_CH6_IRQHandler                    			/* DMA1 channel 6 non-secure interrupt                                       */
  .word	DMA1_CH7_IRQHandler                    			/* DMA1 channel 7 non-secure interrupt                                       */
  .word	ADC_IRQHandler                         			/* ADC global interrupt                                                      */
  .word	DAC_IRQHandler                         			/* DAC global interrupt                                                      */
  .word	0                                      			/* Reserved                                                                  */
  .word	COMP_IRQHandler                        			/* COMP2 and COMP1 interrupt through EXTI[22:21]                             */
  .word	EXTI9_5_IRQHandler                     			/* EXTI line 9_5 interrupt through EXTI                                      */
  .word	TIM1_BRK_IRQHandler                    			/* Timer 1 break interrupt                                                   */
  .word	TIM1_UP_IRQHandler                     			/* Timer 1 Update                                                            */
  .word	TIM1_TRG_COM_IRQHandler                			/* Timer 1 trigger and communication                                         */
  .word	TIM1_CC_IRQHandler                     			/* Timer 1 capture compare interrupt                                         */
  .word	TIM2_IRQHandler                        			/* Timer 2 global interrupt                                                  */
  .word	TIM16_IRQHandler                       			/* Timer 16 global interrupt                                                 */
  .word	TIM17_IRQHandler                       			/* Timer 17 global interrupt                                                 */
  .word	I2C1_EV_IRQHandler                     			/* I2C1 event interrupt                                                      */
  .word	I2C1_ER_IRQHandler                     			/* I2C1 event interrupt                                                      */
  .word	I2C2_EV_IRQHandler                     			/* I2C2 event interrupt                                                      */
  .word	I2C2_ER_IRQHandler                     			/* I2C2 error interrupt                                                      */
  .word	SPI1_IRQHandler                        			/* SPI 1 global interrupt                                                    */
  .word	SPI2S2_IRQHandler                      			/* SPI2S2 global interrupt                                                   */
  .word	USART1_IRQHandler                      			/* USART1 global interrupt                                                   */
  .word	USART2_IRQHandler                      			/* USART2 global interrupt                                                   */
  .word	LPUART1_IRQHandler                     			/* LPUART1 global interrupt                                                  */
  .word	LPTIM1_IRQHandler                      			/* LPtimer 1 global interrupt                                                */
  .word	LPTIM2_IRQHandler                      			/* LPtimer 2 global interrupt                                                */
  .word	EXTI15_10_IRQHandler                   			/* EXTI line 15_10] interrupt through EXTI                                   */
  .word	RTC_ALARM_IRQHandler                   			/* RTC alarms A and B interrupt                                              */
  .word	LPTIM3_IRQHandler                      			/* LPtimer 3 global interrupt                                                */
  .word	0                                      			/* Reserved                                                                  */
  .word	0                                      			/* Reserved                                                                  */
  .word	0                                      			/* Reserved                                                                  */
  .word	HSEM_IRQHandler                        			/* Semaphore interrupt 0 to CPU                                              */
  .word	I2C3_EV_IRQHandler                     			/* I2C3 event interrupt                                                      */
  .word	I2C3_ER_IRQHandler                     			/* I2C3 error interrupt                                                      */
  .word	Radio_IRQ_Busy_IRQHandler              			/* Radio IRQs, RFBUSY interrupt through EXTI                                 */
  .word	AES_IRQHandler                         			/* AES global interrupt                                                      */
  .word	True_RNG_IRQHandler                    			/* True random number generator interrupt                                    */
  .word	PKA_IRQHandler                         			/* Private key accelerator interrupt                                         */
  .word	DMA2_CH1_IRQHandler                    			/* DMA2 channel 1 non-secure interrupt                                       */
  .word	DMA2_CH2_IRQHandler                    			/* DMA2 channel 2 non-secure interrupt                                       */
  .word	DMA2_CH3_IRQHandler                    			/* DMA2 channel 3 non-secure interrupt                                       */
  .word	DMA2_CH4_IRQHandler                    			/* DMA2 channel 4 non-secure interrupt                                       */
  .word	DMA2_CH5_IRQHandler                    			/* DMA2 channel 5 non-secure interrupt                                       */
  .word	DMA2_CH6_IRQHandler                    			/* DMA2 channel 6 non-secure interrupt                                       */
  .word	DMA2_CH7_IRQHandler                    			/* DMA2 channel 7 non-secure interrupt                                       */
  .word	DMAMUX1_OVR_IRQHandler                 			/* DMAMUX1 overrun interrupt                                                 */

/*******************************************************************************
*
* Provide weak aliases for each Exception handler to the Default_Handler.
* As they are weak aliases, any function with the same name will override
* this definition.
*
*******************************************************************************/

	.weak	NMI_Handler
	.thumb_set NMI_Handler,Default_Handler

	.weak	HardFault_Handler
	.thumb_set HardFault_Handler,Default_Handler

	.weak	MemManage_Handler
	.thumb_set MemManage_Handler,Default_Handler

	.weak	BusFault_Handler
	.thumb_set BusFault_Handler,Default_Handler

	.weak	UsageFault_Handler
	.thumb_set UsageFault_Handler,Default_Handler

	.weak	SVC_Handler
	.thumb_set SVC_Handler,Default_Handler

	.weak	DebugMon_Handler
	.thumb_set DebugMon_Handler,Default_Handler

	.weak	PendSV_Handler
	.thumb_set PendSV_Handler,Default_Handler

	.weak	SysTick_Handler
	.thumb_set SysTick_Handler,Default_Handler

	.weak	WWDG_IRQHandler
	.thumb_set WWDG_IRQHandler,Default_Handler

	.weak	PVD_PVM_3_IRQHandler
	.thumb_set PVD_PVM_3_IRQHandler,Default_Handler

	.weak	TAMP_RTCSTAMP_LSECSS_RTCSSRU_IRQHandler
	.thumb_set TAMP_RTCSTAMP_LSECSS_RTCSSRU_IRQHandler,Default_Handler

	.weak	RTC_WKUP_IRQHandler
	.thumb_set RTC_WKUP_IRQHandler,Default_Handler

	.weak	FLASH_IRQHandler
	.thumb_set FLASH_IRQHandler,Default_Handler

	.weak	RCC_IRQHandler
	.thumb_set RCC_IRQHandler,Default_Handler

	.weak	EXTI0_IRQHandler
	.thumb_set EXTI0_IRQHandler,Default_Handler

	.weak	EXTI1_IRQHandler
	.thumb_set EXTI1_IRQHandler,Default_Handler

	.weak	EXTI2_IRQHandler
	.thumb_set EXTI2_IRQHandler,Default_Handler

	.weak	EXTI3_IRQHandler
	.thumb_set EXTI3_IRQHandler,Default_Handler

	.weak	EXTI4_IRQHandler
	.thumb_set EXTI4_IRQHandler,Default_Handler

	.weak	DMA1_CH1_IRQHandler
	.thumb_set DMA1_CH1_IRQHandler,Default_Handler

	.weak	DMA1_CH2_IRQHandler
	.thumb_set DMA1_CH2_IRQHandler,Default_Handler

	.weak	DMA1_CH3_IRQHandler
	.thumb_set DMA1_CH3_IRQHandler,Default_Handler

	.weak	DMA1_CH4_IRQHandler
	.thumb_set DMA1_CH4_IRQHandler,Default_Handler

	.weak	DMA1_CH5_IRQHandler
	.thumb_set DMA1_CH5_IRQHandler,Default_Handler

	.weak	DMA1_CH6_IRQHandler
	.thumb_set DMA1_CH6_IRQHandler,Default_Handler

	.weak	DMA1_CH7_IRQHandler
	.thumb_set DMA1_CH7_IRQHandler,Default_Handler

	.weak	ADC_IRQHandler
	.thumb_set ADC_IRQHandler,Default_Handler

	.weak	DAC_IRQHandler
	.thumb_set DAC_IRQHandler,Default_Handler

	.weak	COMP_IRQHandler
	.thumb_set COMP_IRQHandler,Default_Handler

	.weak	EXTI9_5_IRQHandler
	.thumb_set EXTI9_5_IRQHandler,Default_Handler

	.weak	TIM1_BRK_IRQHandler
	.thumb_set TIM1_BRK_IRQHandler,Default_Handler

	.weak	TIM1_UP_IRQHandler
	.thumb_set TIM1_UP_IRQHandler,Default_Handler

	.weak	TIM1_TRG_COM_IRQHandler
	.thumb_set TIM1_TRG_COM_IRQHandler,Default_Handler

	.weak	TIM1_CC_IRQHandler
	.thumb_set TIM1_CC_IRQHandler,Default_Handler

	.weak	TIM2_IRQHandler
	.thumb_set TIM2_IRQHandler,Default_Handler

	.weak	TIM16_IRQHandler
	.thumb_set TIM16_IRQHandler,Default_Handler

	.weak	TIM17_IRQHandler
	.thumb_set TIM17_IRQHandler,Default_Handler

	.weak	I2C1_EV_IRQHandler
	.thumb_set I2C1_EV_IRQHandler,Default_Handler

	.weak	I2C1_ER_IRQHandler
	.thumb_set I2C1_ER_IRQHandler,Default_Handler

	.weak	I2C2_EV_IRQHandler
	.thumb_set I2C2_EV_IRQHandler,Default_Handler

	.weak	I2C2_ER_IRQHandler
	.thumb_set I2C2_ER_IRQHandler,Default_Handler

	.weak	SPI1_IRQHandler
	.thumb_set SPI1_IRQHandler,Default_Handler

	.weak	SPI2S2_IRQHandler
	.thumb_set SPI2S2_IRQHandler,Default_Handler

	.weak	USART1_IRQHandler
	.thumb_set USART1_IRQHandler,Default_Handler

	.weak	USART2_IRQHandler
	.thumb_set USART2_IRQHandler,Default_Handler

	.weak	LPUART1_IRQHandler
	.thumb_set LPUART1_IRQHandler,Default_Handler

	.weak	LPTIM1_IRQHandler
	.thumb_set LPTIM1_IRQHandler,Default_Handler

	.weak	LPTIM2_IRQHandler
	.thumb_set LPTIM2_IRQHandler,Default_Handler

	.weak	EXTI15_10_IRQHandler
	.thumb_set EXTI15_10_IRQHandler,Default_Handler

	.weak	RTC_ALARM_IRQHandler
	.thumb_set RTC_ALARM_IRQHandler,Default_Handler

	.weak	LPTIM3_IRQHandler
	.thumb_set LPTIM3_IRQHandler,Default_Handler

	.weak	HSEM_IRQHandler
	.thumb_set HSEM_IRQHandler,Default_Handler

	.weak	I2C3_EV_IRQHandler
	.thumb_set I2C3_EV_IRQHandler,Default_Handler

	.weak	I2C3_ER_IRQHandler
	.thumb_set I2C3_ER_IRQHandler,Default_Handler

	.weak	Radio_IRQ_Busy_IRQHandler
	.thumb_set Radio_IRQ_Busy_IRQHandler,Default_Handler

	.weak	AES_IRQHandler
	.thumb_set AES_IRQHandler,Default_Handler

	.weak	True_RNG_IRQHandler
	.thumb_set True_RNG_IRQHandler,Default_Handler

	.weak	PKA_IRQHandler
	.thumb_set PKA_IRQHandler,Default_Handler

	.weak	DMA2_CH1_IRQHandler
	.thumb_set DMA2_CH1_IRQHandler,Default_Handler

	.weak	DMA2_CH2_IRQHandler
	.thumb_set DMA2_CH2_IRQHandler,Default_Handler

	.weak	DMA2_CH3_IRQHandler
	.thumb_set DMA2_CH3_IRQHandler,Default_Handler

	.weak	DMA2_CH4_IRQHandler
	.thumb_set DMA2_CH4_IRQHandler,Default_Handler

	.weak	DMA2_CH5_IRQHandler
	.thumb_set DMA2_CH5_IRQHandler,Default_Handler

	.weak	DMA2_CH6_IRQHandler
	.thumb_set DMA2_CH6_IRQHandler,Default_Handler

	.weak	DMA2_CH7_IRQHandler
	.thumb_set DMA2_CH7_IRQHandler,Default_Handler

	.weak	DMAMUX1_OVR_IRQHandler
	.thumb_set DMAMUX1_OVR_IRQHandler,Default_Handler

	.weak	SystemInit

/************************ (C) COPYRIGHT STMicroelectonics *****END OF FILE****/
