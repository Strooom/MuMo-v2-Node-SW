#include "main.h"
#include <gpio.hpp>
#include <logging.hpp>        // logging to SWO and/or UART
#include <power.hpp>
#include <version.hpp>
#include <display.hpp>
#include <graphics.hpp>
#include <ux.hpp>
#include <stdio.h>
#include <maincontroller.hpp>
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <sensordevicecollection.hpp>
#include <buildinfo.hpp>
#include <lorawan.hpp>
#include <realtimeclock.hpp>
#include <uniqueid.hpp>
#include <settingscollection.hpp>
#include <measurementcollection.hpp>
#include <uart1.hpp>
#include <spi.hpp>
#include <i2c.hpp>
#include <bme680.hpp>
#include <tsl2591.hpp>
#include <sht40.hpp>
#include <battery.hpp>
#include <lptim.hpp>
#include <qrcode.hpp>
#include <hexascii.hpp>
#include <mcutype.hpp>
#include <sx126x.hpp>

ADC_HandleTypeDef hadc;
CRYP_HandleTypeDef hcryp;
__ALIGN_BEGIN static const uint32_t pKeyAES[4] __ALIGN_END = {0x00000000, 0x00000000, 0x00000000, 0x00000000};
I2C_HandleTypeDef hi2c2;
LPTIM_HandleTypeDef hlptim1;
RNG_HandleTypeDef hrng;
RTC_HandleTypeDef hrtc;
SPI_HandleTypeDef hspi2;
SUBGHZ_HandleTypeDef hsubghz;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
circularBuffer<applicationEvent, 16U> applicationEventBuffer;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_GPIO_Init(void);
static void MX_RTC_Init(void);
void MX_USART2_UART_Init(void);
void MX_SPI2_Init(void);
void MX_ADC_Init(void);
void MX_I2C2_Init(void);
void MX_AES_Init(void);
void MX_RNG_Init(void);
static void MX_LPTIM1_Init(void);
// static void MX_SUBGHZ_Init(void);
void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
void executeRomBootloader();

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_MSI);
    gpio::initialize();
    if (gpio::isDebugProbePresent()) {
        HAL_Delay(3000);
    } else {
        gpio::disableGpio(gpio::group::debugPort);
    }

    if (power::hasUsbPower()) {
        HAL_RCC_DeInit();
        HAL_DeInit();
        executeRomBootloader();
    }

    version::initialize();
    uart1::wakeUp();
    logging::enable(logging::destination::uart1);
    gpio::enableGpio(gpio::group::other);

    MX_RTC_Init();
    MX_ADC_Init();

    // MX_SPI2_Init();
    // MX_AES_Init();
    // MX_RNG_Init();
    // MX_LPTIM1_Init();

    uint32_t loopCount{0};

    while (true) {
        // HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);

        logging::snprintf("\n\n\n%d\n", loopCount);
        logging::snprintf("https://github.com/Strooom - %s\n", version::getIsVersionAsString());
        logging::snprintf("%s %s build - %s\n", toString(version::getBuildEnvironment()), toString(version::getBuildType()), buildInfo::buildTimeStamp);
        logging::snprintf("Creative Commons 4.0 - BY-NC-SA\n\n");
        char tmpKeyAsHexAscii[17];
        hexAscii::uint64ToHexString(tmpKeyAsHexAscii, uniqueId::get());
        logging::snprintf("UID     : %s\n", tmpKeyAsHexAscii);

        spi::wakeUp();
        display::detectPresence();
        logging::snprintf("Display : %s\n", display::isPresent() ? "present" : "not present");
        spi::goSleep();

        i2c::wakeUp();
        logging::snprintf("EEPROM  : %d * 64K present\n", nonVolatileStorage::isPresent());
        logging::snprintf("BME68X  : %s\n", bme680::isPresent() ? "present" : "not present");
        logging::snprintf("TSL2591 : %s\n", tsl2591::isPresent() ? "present" : "not present");
        logging::snprintf("SHT40   : %s\n", sht40::isPresent() ? "present" : "not present");
        batteryType theBatteryType = static_cast<batteryType>(settingsCollection::read<uint8_t>(settingsCollection::settingIndex::batteryType));
        logging::snprintf("Battery : %s (%d)\n", toString(theBatteryType), static_cast<uint8_t>(theBatteryType));
        mcuType theMcuType = static_cast<mcuType>(settingsCollection::read<uint8_t>(settingsCollection::settingIndex::mcuType));
        logging::snprintf("Radio   : %s (%d)\n", toString(theMcuType), static_cast<uint8_t>(theMcuType));
        i2c::goSleep();

        if (display::isPresent()) {
            spi::wakeUp();
            screen::setType(screenType::logo);
            screen::update();
            spi::goSleep();
        }

        logging::snprintf("goSleep...\n");
        uart1::goSleep();
        mainController::mcuStop2();
        uart1::wakeUp();
        logging::snprintf("... wakeUp\n");

        loopCount++;
    }
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
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
    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.LSEState            = RCC_LSE_ON;
    RCC_OscInitStruct.MSIState            = RCC_MSI_ON;
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

/**
 * @brief ADC Initialization Function
 * @param None
 * @retval None
 */
void MX_ADC_Init(void) {
    /* USER CODE BEGIN ADC_Init 0 */

    /* USER CODE END ADC_Init 0 */

    /* USER CODE BEGIN ADC_Init 1 */

    /* USER CODE END ADC_Init 1 */

    /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
     */
    hadc.Instance                   = ADC;
    hadc.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV1;
    hadc.Init.Resolution            = ADC_RESOLUTION_12B;
    hadc.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
    hadc.Init.ScanConvMode          = ADC_SCAN_DISABLE;
    hadc.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;
    hadc.Init.LowPowerAutoWait      = DISABLE;
    hadc.Init.LowPowerAutoPowerOff  = ENABLE;
    hadc.Init.ContinuousConvMode    = DISABLE;
    hadc.Init.NbrOfConversion       = 1;
    hadc.Init.DiscontinuousConvMode = DISABLE;
    hadc.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
    hadc.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc.Init.DMAContinuousRequests = DISABLE;
    hadc.Init.Overrun               = ADC_OVR_DATA_PRESERVED;
    hadc.Init.SamplingTimeCommon1   = ADC_SAMPLETIME_79CYCLES_5;
    hadc.Init.SamplingTimeCommon2   = ADC_SAMPLETIME_79CYCLES_5;
    hadc.Init.OversamplingMode      = DISABLE;
    hadc.Init.TriggerFrequencyMode  = ADC_TRIGGER_FREQ_HIGH;
    if (HAL_ADC_Init(&hadc) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN ADC_Init 2 */
    HAL_ADCEx_Calibration_Start(&hadc);

    /* USER CODE END ADC_Init 2 */
}

/**
 * @brief AES Initialization Function
 * @param None
 * @retval None
 */
void MX_AES_Init(void) {
    /* USER CODE BEGIN AES_Init 0 */

    /* USER CODE END AES_Init 0 */

    /* USER CODE BEGIN AES_Init 1 */

    /* USER CODE END AES_Init 1 */
    hcryp.Instance             = AES;
    hcryp.Init.DataType        = CRYP_DATATYPE_8B;
    hcryp.Init.KeySize         = CRYP_KEYSIZE_128B;
    hcryp.Init.pKey            = (uint32_t *)pKeyAES;
    hcryp.Init.Algorithm       = CRYP_AES_ECB;
    hcryp.Init.DataWidthUnit   = CRYP_DATAWIDTHUNIT_WORD;
    hcryp.Init.HeaderWidthUnit = CRYP_HEADERWIDTHUNIT_WORD;
    hcryp.Init.KeyIVConfigSkip = CRYP_KEYIVCONFIG_ALWAYS;
    if (HAL_CRYP_Init(&hcryp) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN AES_Init 2 */

    /* USER CODE END AES_Init 2 */
}

/**
 * @brief I2C2 Initialization Function
 * @param None
 * @retval None
 */
void MX_I2C2_Init(void) {
    /* USER CODE BEGIN I2C2_Init 0 */

    /* USER CODE END I2C2_Init 0 */

    /* USER CODE BEGIN I2C2_Init 1 */

    /* USER CODE END I2C2_Init 1 */
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

/**
 * @brief LPTIM1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_LPTIM1_Init(void) {
    /* USER CODE BEGIN LPTIM1_Init 0 */

    /* USER CODE END LPTIM1_Init 0 */

    /* USER CODE BEGIN LPTIM1_Init 1 */

    /* USER CODE END LPTIM1_Init 1 */
    hlptim1.Instance             = LPTIM1;
    hlptim1.Init.Clock.Source    = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC;
    hlptim1.Init.Clock.Prescaler = LPTIM_PRESCALER_DIV8;
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

/**
 * @brief RNG Initialization Function
 * @param None
 * @retval None
 */
void MX_RNG_Init(void) {
    /* USER CODE BEGIN RNG_Init 0 */

    /* USER CODE END RNG_Init 0 */

    /* USER CODE BEGIN RNG_Init 1 */

    /* USER CODE END RNG_Init 1 */
    hrng.Instance                 = RNG;
    hrng.Init.ClockErrorDetection = RNG_CED_DISABLE;
    if (HAL_RNG_Init(&hrng) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN RNG_Init 2 */

    /* USER CODE END RNG_Init 2 */
}

/**
 * @brief RTC Initialization Function
 * @param None
 * @retval None
 */
static void MX_RTC_Init(void) {
    /* USER CODE BEGIN RTC_Init 0 */

    /* USER CODE END RTC_Init 0 */

    // RTC_TimeTypeDef sTime     = {0};
    // RTC_DateTypeDef sDate     = {0};
    // RTC_TamperTypeDef sTamper = {0};

    /* USER CODE BEGIN RTC_Init 1 */

    /* USER CODE END RTC_Init 1 */

    /** Initialize RTC Only
     */
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

   if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 61439, RTC_WAKEUPCLOCK_RTCCLK_DIV16, 0) != HAL_OK) {
        // if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 10000, RTC_WAKEUPCLOCK_RTCCLK_DIV16, 0) != HAL_OK) {
        Error_Handler();
    }

    /* USER CODE BEGIN Check_RTC_BKUP */
    HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 10, 0);
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
    /* USER CODE END Check_RTC_BKUP */
}

/**
 * @brief SPI2 Initialization Function
 * @param None
 * @retval None
 */
void MX_SPI2_Init(void) {
    /* USER CODE BEGIN SPI2_Init 0 */

    /* USER CODE END SPI2_Init 0 */

    /* USER CODE BEGIN SPI2_Init 1 */

    /* USER CODE END SPI2_Init 1 */
    /* SPI2 parameter configuration*/
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
    /* USER CODE BEGIN SPI2_Init 2 */

    /* USER CODE END SPI2_Init 2 */
}

// /**
//  * @brief SUBGHZ Initialization Function
//  * @param None
//  * @retval None
//  */
// static void MX_SUBGHZ_Init(void) {
//     /* USER CODE BEGIN SUBGHZ_Init 0 */

//     /* USER CODE END SUBGHZ_Init 0 */

//     /* USER CODE BEGIN SUBGHZ_Init 1 */

//     /* USER CODE END SUBGHZ_Init 1 */
//     hsubghz.Init.BaudratePrescaler = SUBGHZSPI_BAUDRATEPRESCALER_2;
//     if (HAL_SUBGHZ_Init(&hsubghz) != HAL_OK) {
//         Error_Handler();
//     }
//     /* USER CODE BEGIN SUBGHZ_Init 2 */

//     /* USER CODE END SUBGHZ_Init 2 */
// }

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
void MX_USART1_UART_Init(void) {
    /* USER CODE BEGIN USART1_Init 0 */

    /* USER CODE END USART1_Init 0 */

    /* USER CODE BEGIN USART1_Init 1 */

    /* USER CODE END USART1_Init 1 */
    huart1.Instance                    = USART1;
    huart1.Init.BaudRate               = 115200;
    huart1.Init.WordLength             = UART_WORDLENGTH_8B;
    huart1.Init.StopBits               = UART_STOPBITS_1;
    huart1.Init.Parity                 = UART_PARITY_NONE;
    huart1.Init.Mode                   = UART_MODE_TX;
    huart1.Init.HwFlowCtl              = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling           = UART_OVERSAMPLING_16;
    huart1.Init.OneBitSampling         = UART_ONE_BIT_SAMPLE_DISABLE;
    huart1.Init.ClockPrescaler         = UART_PRESCALER_DIV1;
    huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart1) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN USART1_Init 2 */

    /* USER CODE END USART1_Init 2 */
}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
void MX_USART2_UART_Init(void) {
    /* USER CODE BEGIN USART2_Init 0 */

    /* USER CODE END USART2_Init 0 */

    /* USER CODE BEGIN USART2_Init 1 */

    /* USER CODE END USART2_Init 1 */
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
    /* USER CODE BEGIN USART2_Init 2 */

    /* USER CODE END USART2_Init 2 */
}

void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* USER CODE BEGIN MX_GPIO_Init_1 */
    /* USER CODE END MX_GPIO_Init_1 */

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /*Configure GPIO pin : PB3 */
    GPIO_InitStruct.Pin  = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* EXTI interrupt init*/
    HAL_NVIC_SetPriority(EXTI3_IRQn, 7, 0);
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);

    /* USER CODE BEGIN MX_GPIO_Init_2 */
    /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_SUBGHZ_TxCpltCallback(SUBGHZ_HandleTypeDef *hsubghz) {
    applicationEventBuffer.push(applicationEvent::sx126xTxComplete);
}

void HAL_SUBGHZ_RxCpltCallback(SUBGHZ_HandleTypeDef *hsubghz) {
    applicationEventBuffer.push(applicationEvent::sx126xRxComplete);
}

void HAL_SUBGHZ_RxTxTimeoutCallback(SUBGHZ_HandleTypeDef *hsubghz) {
    applicationEventBuffer.push(applicationEvent::sx126xTimeout);
}

void HAL_RTCEx_Tamper3EventCallback(RTC_HandleTypeDef *hrtc) {
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    applicationEventBuffer.push(applicationEvent::applicationButtonPressed);
}

void executeRomBootloader() {
    void (*SysMemBootJump)(void);
    volatile uint32_t BootAddr = 0x1FFF0000;

    __disable_irq();
    SysTick->CTRL = 0;
    HAL_RCC_DeInit();
    for (uint8_t i = 0; i < sizeof(NVIC->ICER) / sizeof(NVIC->ICER[0]); i++) {
        NVIC->ICER[i] = 0xFFFFFFFF;
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }
    __enable_irq();

    SysMemBootJump = (void (*)(void))(*((uint32_t *)((BootAddr + 4))));

    __set_MSP(*(uint32_t *)BootAddr);        // Set the main stack pointer to the boot loader stack

    SysMemBootJump();
    while (1) {
    }
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
