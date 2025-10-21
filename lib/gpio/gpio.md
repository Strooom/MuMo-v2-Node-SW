# Steps needed to add interrupt to a GPIO pin

1. set the mode of the pin example : 
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;

2. set the interrupt priority and enable the interrupt : 
    HAL_NVIC_SetPriority(EXTI4_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);

3. declare the related interrupt handler in stm32wlxx_it.h
    void EXTI4_IRQHandler(void);
    otherwise the linker will not link your interrupt handler because of the C++ name mangling


3. implement the related interrupt handler in stm32wlxx_it.cpp
    void EXTI4_IRQHandler(void) {
        HAL_GPIO_EXTI_IRQHandler(usbPowerPresent_Pin);
    }