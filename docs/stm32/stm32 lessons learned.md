* When using the debugger with a ST-Link, the connection of the debugger SW to the ST-Link/STM32 takes several seconds, in which the STM32 is reset a few times. If the STM32 goes into sleep in this time (or changes the debugport pins, ..), then the debugger will not connect. Solution is to add a HAL_Delay(7000) at the very beginning of the code.

* When after debugging or programming, the device is reset with RST button, the debug port is NOT disabled and stays active, resulting in increased power consumption. A power cycle reset (with debug probe removed) is needed to get rid of this and ensure lowest power consumption.

* 