// #############################################################################
// ### This file is part of the source code for the MuMo project             ###
// ### https://github.com/Strooom/MuMo-v2-Node-SW                            ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <stdint.h>

#include "logging.hpp"               // logging to SWO and/or UART
#include "circularbuffer.h"        // needed for global applicationEventBuffer and loraWanEventBuffer
#include "applicationevent.h"
#include "version.h"
#include "lorawanevent.h"
#include "maincontroller.h"
#include "power.h"
#include "cli.h"
#include "lorawan.h"
#include "gpio.h"
#include "display.h"
#include "main.h"                  // required for access to STM32 HAL
#include "utilities_conf.h"        // required for access to STM32 HAL

ADC_HandleTypeDef hadc;
CRYP_HandleTypeDef hcryp;
I2C_HandleTypeDef hi2c2;
LPTIM_HandleTypeDef hlptim1;
RNG_HandleTypeDef hrng;
RTC_HandleTypeDef hrtc;
SPI_HandleTypeDef hspi2;
SUBGHZ_HandleTypeDef hsubghz;
// UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;        // UART2 on PA2 (TX) and PA3 (RX) is connected to the USB-UART bridge

circularBuffer<applicationEvent, 16U> applicationEventBuffer;
circularBuffer<loRaWanEvent, 16U> loraWanEventBuffer;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_GPIO_LowPower(bool debugProbePresent);
static void MX_ADC_Init(void);
static void MX_AES_Init(void);
static void MX_I2C2_Init(void);
static void MX_LPTIM1_Init(void);
static void MX_RNG_Init(void);
static void MX_RTC_Init(void);
static void MX_SPI2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);

int main(void) {
    HAL_Init();
    HAL_Delay(5000U);                                                 // This initial delay prevents the MCU from going into sleep immediately, so the debugger is able to connect
    SystemClock_Config();                                             // first thing to do is configure the clock : 16 MHz MSI for MCU and 32768 kHz LSE for RTC
    __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_MSI);        // set the clock for the wakeup from STOP mode to MSI as well
    MX_USART2_UART_Init();

    // Configure logging

    if ((CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) == 0x0001) {        // if there is a SWD debugprobe connected...
        logging::enable(logging::destination::debugProbe);                     // enable the output to SWO
    }
    gpio::enableGpio(gpio::group::usbPresent);
    if (power::hasUsbPower()) {                             // if there is USB power...
        logging::enable(logging::destination::uart);        // enable the output to UART
    }

    // Show the version of the software

    version::setIsVersion();
    logging::snprintf("MuMo v2 - %s - Boot\n", version::getIsVersionAsString());
    logging::snprintf("Creative Commons 4.0 - BY-NC-SA\n");

    if (logging::isActive(logging::destination::debugProbe)) {
        LL_DBGMCU_EnableDBGStopMode();
        logging::snprintf("debugProbe connected\n");
    } else {
        LL_DBGMCU_DisableDBGStopMode();                   // no debugging in low power -> the MCU will really stop the clock
        gpio::disableGpio(gpio::group::debugPort);        // these IOs are enabled by default after reset, but as there is no debug probe, we disable them to reduce power consumption
    }

    if (logging::isActive(logging::destination::uart)) {
        logging::snprintf("USB connected\n");
    }

    logging::enable(logging::source::criticalError);
    logging::enable(logging::source::error);
    // logging::enable(logging::source::lorawanSettings);
    // logging::enable(logging::source::lorawanState);
    // logging::enablelogging::source(logging::source::lorawanTiming);
    // logging::enablelogging::source(logging::source::lorawanEvents);
    // logging::enablelogging::source(logging::source::lorawanMac);
    // logging::enable(logging::source::sensorEvents);

    // gpio::enableGpio(gpio::group::i2c);
    // gpio::enableGpio(gpio::group::writeProtect);
    // gpio::enableGpio(gpio::group::rfControl);

    //MX_RTC_Init();
    //MX_LPTIM1_Init();
    //MX_RNG_Init();
    //MX_I2C2_Init();
    //MX_AES_Init();

    gpio::enableGpio(gpio::group::spiDisplay);
    if (display::isPresent()) {
        logging::snprintf("Display present\n");
    } else {
        logging::snprintf("Display not present\n");
    }


    mainController::initialize();

    while (1) {
        // Detect removal or insertion of USB cable/power
        if (power::isUsbConnected()) {
            applicationEventBuffer.push(applicationEvent::usbConnected);
        }
        if (power::isUsbRemoved()) {
            applicationEventBuffer.push(applicationEvent::usbRemoved);
        }

        cli::handleRxEvent();
        LoRaWAN::handleEvents();
        mainController::handleEvents();

        if (!power::hasUsbPower()) {        // When no external USB power, go into sleep
            gpio::disableGpio(gpio::group::usbPresent);
            HAL_I2C_DeInit(&hi2c2);
            UTILS_ENTER_CRITICAL_SECTION();        // mask interrupts TODO : document why even if interrupts are masked, the RTC still wakes up the MCU. What about other interrupts to wake up ?

            //     if (LoRaWAN::theTxRxCycleState == txRxCycleState::idle) {                          // If the LoRaWAN stack is idle...
            if (applicationEventBuffer.isEmpty() && loraWanEventBuffer.isEmpty()) {        // If no events are pending in any of the eventBuffers...
                logging::snprintf("goSleep...\n");                                         //
                HAL_SuspendTick();                                                         // stop Systick
                HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);                               // go into Sleep - STOP2
                HAL_ResumeTick();                                                          // re-enable Systick
                logging::snprintf("... wakeUp\n");                                         //
            }                                                                              //
            //     }                                                                                  //

            UTILS_EXIT_CRITICAL_SECTION();        // re-enable interrupts
            MX_I2C2_Init();
            gpio::enableGpio(gpio::group::usbPresent);
        }
    }
}

void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure LSE Drive Capability
     */
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

    /** Configure the main internal regulator output voltage
     */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.LSEState            = RCC_LSE_ON;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.MSIState            = RCC_MSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.MSIClockRange       = RCC_MSIRANGE_8;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers
     */
    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK3 | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_MSI;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.AHBCLK3Divider = RCC_SYSCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
        Error_Handler();
    }
}

static void MX_AES_Init(void) {
    __ALIGN_BEGIN static uint32_t pKeyAES[4] __ALIGN_END = {0x00000000, 0x00000000, 0x00000000, 0x00000000};

    hcryp.Instance             = AES;
    hcryp.Init.DataType        = CRYP_DATATYPE_32B;
    hcryp.Init.KeySize         = CRYP_KEYSIZE_128B;
    hcryp.Init.pKey            = pKeyAES;
    hcryp.Init.Algorithm       = CRYP_AES_ECB;
    hcryp.Init.DataWidthUnit   = CRYP_DATAWIDTHUNIT_WORD;
    hcryp.Init.HeaderWidthUnit = CRYP_HEADERWIDTHUNIT_WORD;
    hcryp.Init.KeyIVConfigSkip = CRYP_KEYIVCONFIG_ALWAYS;
    if (HAL_CRYP_Init(&hcryp) != HAL_OK) {
        Error_Handler();
    }
}

static void MX_I2C2_Init(void) {
    hi2c2.Instance              = I2C2;
    hi2c2.Init.Timing           = 0x0010061A;
    hi2c2.Init.OwnAddress1      = 0;
    hi2c2.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
    hi2c2.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
    hi2c2.Init.OwnAddress2      = 0;
    hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c2.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
    hi2c2.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c2) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Analogue filter
     */
    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Digital filter
     */
    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN I2C2_Init 2 */

    /* USER CODE END I2C2_Init 2 */
}

static void MX_LPTIM1_Init(void) {
    /* USER CODE BEGIN LPTIM1_Init 0 */

    /* USER CODE END LPTIM1_Init 0 */

    /* USER CODE BEGIN LPTIM1_Init 1 */

    /* USER CODE END LPTIM1_Init 1 */
    hlptim1.Instance             = LPTIM1;
    hlptim1.Init.Clock.Source    = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC;
    hlptim1.Init.Clock.Prescaler = LPTIM_PRESCALER_DIV16;
    hlptim1.Init.Trigger.Source  = LPTIM_TRIGSOURCE_SOFTWARE;
    hlptim1.Init.OutputPolarity  = LPTIM_OUTPUTPOLARITY_HIGH;
    hlptim1.Init.UpdateMode      = LPTIM_UPDATE_IMMEDIATE;
    hlptim1.Init.CounterSource   = LPTIM_COUNTERSOURCE_INTERNAL;
    hlptim1.Init.Input1Source    = LPTIM_INPUT1SOURCE_GPIO;
    hlptim1.Init.Input2Source    = LPTIM_INPUT2SOURCE_GPIO;
    if (HAL_LPTIM_Init(&hlptim1) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN LPTIM1_Init 2 */

    /* USER CODE END LPTIM1_Init 2 */
}

static void MX_RNG_Init(void) {
    hrng.Instance                 = RNG;
    hrng.Init.ClockErrorDetection = RNG_CED_ENABLE;
    if (HAL_RNG_Init(&hrng) != HAL_OK) {
        Error_Handler();
    }
}

static void MX_RTC_Init(void) {
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    char timeString[9]  = __TIME__;        // eg "10:20:30"
    uint32_t hourNow    = (timeString[0] - '0') * 10 + (timeString[1] - '0');
    uint32_t minutesNow = (timeString[3] - '0') * 10 + (timeString[4] - '0');
    uint32_t secondsNow = (timeString[6] - '0') * 10 + (timeString[7] - '0');

    hrtc.Instance            = RTC;
    hrtc.Init.HourFormat     = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv   = 127;
    hrtc.Init.SynchPrediv    = 255;
    hrtc.Init.OutPut         = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutRemap    = RTC_OUTPUT_REMAP_NONE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;
    hrtc.Init.OutPutPullUp   = RTC_OUTPUT_PULLUP_NONE;
    hrtc.Init.BinMode        = RTC_BINARY_NONE;
    if (HAL_RTC_Init(&hrtc) != HAL_OK) {
        Error_Handler();
    }

    // Initialize RTC and set the Time and Date
    sTime.Hours          = hourNow;
    sTime.Minutes        = minutesNow;
    sTime.Seconds        = secondsNow;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    // logging::snprintf("Setting RTC to %02u:%02u:%02u\n", hourNow, minutesNow, secondsNow);

    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
        Error_Handler();
    }

    RTC_TimeTypeDef currTime = {0};
    RTC_DateTypeDef currDate = {0};

    HAL_RTC_GetTime(&hrtc, &currTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &currDate, RTC_FORMAT_BIN);
    // logging::snprintf("Time = %02u:%02u:%02u\n", currTime.Hours, currTime.Minutes, currTime.Seconds);

    sDate.WeekDay = RTC_WEEKDAY_MONDAY;
    sDate.Month   = RTC_MONTH_APRIL;
    sDate.Date    = 26;
    sDate.Year    = 23;

    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
        Error_Handler();
    }

    // Enable the WakeUp on RTC tick Interrupt

    if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 61439, RTC_WAKEUPCLOCK_RTCCLK_DIV16,
                                    0) != HAL_OK) {
        Error_Handler();
    }

    //    if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 61439, RTC_WAKEUPCLOCK_RTCCLK_DIV16, 0) != HAL_OK) {
    //        Error_Handler();
    //    }
}

static void MX_SPI2_Init(void) {
    hspi2.Instance               = SPI2;
    hspi2.Init.Mode              = SPI_MODE_MASTER;
    hspi2.Init.Direction         = SPI_DIRECTION_2LINES;
    hspi2.Init.DataSize          = SPI_DATASIZE_8BIT;
    hspi2.Init.CLKPolarity       = SPI_POLARITY_LOW;
    hspi2.Init.CLKPhase          = SPI_PHASE_1EDGE;
    hspi2.Init.NSS               = SPI_NSS_SOFT;
    hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi2.Init.FirstBit          = SPI_FIRSTBIT_MSB;
    hspi2.Init.TIMode            = SPI_TIMODE_DISABLE;
    hspi2.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
    hspi2.Init.CRCPolynomial     = 7;
    hspi2.Init.CRCLength         = SPI_CRC_LENGTH_DATASIZE;
    hspi2.Init.NSSPMode          = SPI_NSS_PULSE_ENABLE;
    if (HAL_SPI_Init(&hspi2) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * @brief SUBGHZ Initialization Function
 * @param None
 * @retval None
 */
void MX_SUBGHZ_Init(void) {
    hsubghz.Init.BaudratePrescaler = SUBGHZSPI_BAUDRATEPRESCALER_2;
    if (HAL_SUBGHZ_Init(&hsubghz) != HAL_OK) {
        Error_Handler();
    }
}

// static void MX_USART1_UART_Init(void) {
//     huart1.Instance                    = USART1;
//     huart1.Init.BaudRate               = 115200;
//     huart1.Init.WordLength             = UART_WORDLENGTH_8B;
//     huart1.Init.StopBits               = UART_STOPBITS_1;
//     huart1.Init.Parity                 = UART_PARITY_NONE;
//     huart1.Init.Mode                   = UART_MODE_TX_RX;
//     huart1.Init.HwFlowCtl              = UART_HWCONTROL_NONE;
//     huart1.Init.OverSampling           = UART_OVERSAMPLING_16;
//     huart1.Init.OneBitSampling         = UART_ONE_BIT_SAMPLE_DISABLE;
//     huart1.Init.ClockPrescaler         = UART_PRESCALER_DIV1;
//     huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
//     if (HAL_UART_Init(&huart1) != HAL_OK) {
//         Error_Handler();
//     }
//     if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK) {
//         Error_Handler();
//     }
//     if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK) {
//         Error_Handler();
//     }
//     if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK) {
//         Error_Handler();
//     }
// }

static void MX_USART2_UART_Init(void) {
    huart2.Instance                    = USART2;
    huart2.Init.BaudRate               = 115200;
    huart2.Init.WordLength             = UART_WORDLENGTH_8B;
    huart2.Init.StopBits               = UART_STOPBITS_1;
    huart2.Init.Parity                 = UART_PARITY_NONE;
    huart2.Init.Mode                   = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl              = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling           = UART_OVERSAMPLING_16;
    huart2.Init.OneBitSampling         = UART_ONE_BIT_SAMPLE_DISABLE;
    huart2.Init.ClockPrescaler         = UART_PRESCALER_DIV1;
    huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart2) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK) {
        Error_Handler();
    }
}

static void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    HAL_GPIO_WritePin(GPIOB, displayDataCommand_Pin | displayChipSelect_Pin,
                      GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, writeProtect_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, displayReset_Pin | rfControl1_Pin | rfControl2_Pin,
                      GPIO_PIN_RESET);

    GPIO_InitStruct.Pin   = displayReset_Pin | rfControl1_Pin | rfControl2_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin  = usbPowerPresent_Pin | displayBusy_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin   = writeProtect_Pin | displayDataCommand_Pin | displayChipSelect_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

static void MX_GPIO_LowPower(bool debugProbePresent) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    HAL_Delay(1U);

    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;

    if (debugProbePresent) {
        GPIO_InitStruct.Pin = 0x9FFF;
    } else {
        GPIO_InitStruct.Pin = 0xFFFF;
    }
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    if (debugProbePresent) {
        GPIO_InitStruct.Pin = 0xFFF7;
    } else {
        GPIO_InitStruct.Pin = 0xFFFF;
    }
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = 0xFFFF;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    HAL_Delay(1U);

    __HAL_RCC_GPIOA_CLK_DISABLE();
    __HAL_RCC_GPIOB_CLK_DISABLE();
    __HAL_RCC_GPIOC_CLK_DISABLE();
}

void Error_Handler(void) {
    __disable_irq();
    while (1) {
    }
}

void HAL_LPTIM_CompareMatchCallback(LPTIM_HandleTypeDef *hlptim) {
    loraWanEventBuffer.push(loRaWanEvent::timeOut);
    logging::snprintf(logging::source::lorawanTiming, "expired = %u\n", HAL_GetTick());
}

void HAL_SUBGHZ_TxCpltCallback(SUBGHZ_HandleTypeDef *hsubghz) {
    HAL_GPIO_WritePin(GPIOA, loraTiming_Pin, GPIO_PIN_RESET);
    loraWanEventBuffer.push(loRaWanEvent::sx126xTxComplete);
}

void HAL_SUBGHZ_RxCpltCallback(SUBGHZ_HandleTypeDef *hsubghz) {
    HAL_GPIO_WritePin(GPIOA, loraTiming_Pin, GPIO_PIN_RESET);
    loraWanEventBuffer.push(loRaWanEvent::sx126xRxComplete);
}

void HAL_SUBGHZ_RxTxTimeoutCallback(SUBGHZ_HandleTypeDef *hsubghz) {
    HAL_GPIO_WritePin(GPIOA, loraTiming_Pin, GPIO_PIN_RESET);
    loraWanEventBuffer.push(loRaWanEvent::sx126xTimeout);
}
