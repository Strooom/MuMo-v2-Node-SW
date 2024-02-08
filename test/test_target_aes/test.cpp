#include <unity.h>
#include "main.h"
#include <circularbuffer.hpp>
#include <applicationevent.hpp>
#include <aeskey.hpp>
#include <aesblock.hpp>
#include <stm32wle5_aes.hpp>        // my own HAL for AES peripheral as the ST HAL is not so user friendly

// https://www.cryptool.org/en/cto/aes-step-by-step

LPTIM_HandleTypeDef hlptim1;
RTC_HandleTypeDef hrtc;
SUBGHZ_HandleTypeDef hsubghz;
UART_HandleTypeDef huart2;
circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void SystemClock_Config(void);
void MX_USART2_UART_Init(void);
void Error_Handler(void);

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void test_initialize() {
    stm32wle5_aes::initialize(aesMode::EBC);
    TEST_ASSERT_EQUAL_UINT32(0x4, AES->CR);
    stm32wle5_aes::initialize(aesMode::CBC);
    TEST_ASSERT_EQUAL_UINT32(0x24, AES->CR);
    stm32wle5_aes::initialize(aesMode::CTR);
    TEST_ASSERT_EQUAL_UINT32(0x44, AES->CR);
}

void test_enable_disable() {
    stm32wle5_aes::initialize(aesMode::EBC);
    stm32wle5_aes::enable();
    TEST_ASSERT_EQUAL_UINT32(0x5, AES->CR);
    stm32wle5_aes::disable();
    TEST_ASSERT_EQUAL_UINT32(0x4, AES->CR);
}

void test_setKey() {
    aesKey testKey;
    testKey.setFromHexString("2b7e151628aed2a6abf7158809cf4f3c");
    stm32wle5_aes::setKey(testKey);
    TEST_IGNORE_MESSAGE("For code tracing only as key-registers cannot be read back...");
}

void test_setInitVector() {
    aesBlock testData;
    testData.setFromHexString("2b7e151628aed2a6abf7158809cf4f3c");
    stm32wle5_aes::setInitializationVector(testData);
    TEST_ASSERT_EQUAL_UINT32(0x09cf4f3c, AES->IVR0);
    TEST_ASSERT_EQUAL_UINT32(0xabf71588, AES->IVR1);
    TEST_ASSERT_EQUAL_UINT32(0x28aed2a6, AES->IVR2);
    TEST_ASSERT_EQUAL_UINT32(0x2b7e1516, AES->IVR3);
}

void test_hwEncryptSingleBlock() {
    aesKey key;
    key.setFromHexString("2b7e151628aed2a6abf7158809cf4f3c");
    aesBlock inputOutput;
    inputOutput.setFromHexString("6bc1bee22e409f96e93d7e117393172a");        // test-vector 1
    inputOutput.encrypt(key);
    aesBlock expectedCypherText;
    expectedCypherText.setFromHexString("3ad77bb40d7a3660a89ecaf32466ef97");
    TEST_ASSERT_TRUE(inputOutput == expectedCypherText);

    inputOutput.setFromHexString("ae2d8a571e03ac9c9eb76fac45af8e51");        // test-vector 2
    inputOutput.encrypt(key);
    expectedCypherText.setFromHexString("f5d3d58503b9699de785895a96fdbaaf");
    TEST_ASSERT_TRUE(inputOutput == expectedCypherText);

    inputOutput.setFromHexString("30c81c46a35ce411e5fbc1191a0a52ef");        // test-vector 3
    inputOutput.encrypt(key);
    expectedCypherText.setFromHexString("43b1cd7f598ece23881b00e3ed030688");
    TEST_ASSERT_TRUE(inputOutput == expectedCypherText);

    inputOutput.setFromHexString("f69f2445df4f9b17ad2b417be66c3710");        // test-vector 4
    inputOutput.encrypt(key);
    expectedCypherText.setFromHexString("7b0c785e27e8ad3f8223207104725dd4");
    TEST_ASSERT_TRUE(inputOutput == expectedCypherText);
}

void test_hwEncryptLoRaWANPayload() {
    aesKey key;
    key.setFromHexString("2b7e151628aed2a6abf7158809cf4f3c");
    aesBlock A1;
    A1.setFromHexString("01000000000078563412ccddeeff0001");
    aesBlock input;
    input.setFromHexString("00000000000000000000000000000000");
    aesBlock output;
    aesBlock toBe;

    stm32wle5_aes::initialize(aesMode::CTR);
    stm32wle5_aes::setKey(key);
    stm32wle5_aes::setInitializationVector(A1);
    stm32wle5_aes::enable();

    // first block

    stm32wle5_aes::write(input);
    while (!stm32wle5_aes::isComputationComplete()) {
    }
    stm32wle5_aes::clearComputationComplete();
    stm32wle5_aes::read(output);
    toBe.setFromHexString("4d9655c553debea0dfa6290bbad3ebc9");
    TEST_ASSERT_TRUE(output == toBe);

    // second block

    stm32wle5_aes::write(input);
    while (!stm32wle5_aes::isComputationComplete()) {
    }
    stm32wle5_aes::clearComputationComplete();
    stm32wle5_aes::read(output);
    toBe.setFromHexString("9376ed97203ab35f8bf1abd8b63e972e");
    TEST_ASSERT_TRUE(output == toBe);

    // third block

    stm32wle5_aes::write(input);
    while (!stm32wle5_aes::isComputationComplete()) {
    }
    stm32wle5_aes::clearComputationComplete();
    stm32wle5_aes::read(output);
    toBe.setFromHexString("6b0c10e37d694115c32d4f7a807db808");
    TEST_ASSERT_TRUE(output == toBe);
}

void test_hwCalculateMic() {
    aesKey key;
    key.setFromHexString("2b7e151628aed2a6abf7158809cf4f3c");
    aesBlock input;
    input.setFromHexString("00000000000000000000000000000000");
    aesBlock output;
    aesBlock toBe;

    stm32wle5_aes::initialize(aesMode::CBC);
    stm32wle5_aes::setKey(key);
    stm32wle5_aes::setInitializationVector(input);
    stm32wle5_aes::enable();

    // first block

    stm32wle5_aes::write(input);
    while (!stm32wle5_aes::isComputationComplete()) {
    }
    stm32wle5_aes::clearComputationComplete();
    stm32wle5_aes::read(output);
    toBe.setFromHexString("7df76b0c1ab899b33e42f047b91b546f");
    TEST_ASSERT_TRUE(output == toBe);

    // second block

    stm32wle5_aes::write(input);
    while (!stm32wle5_aes::isComputationComplete()) {
    }
    stm32wle5_aes::clearComputationComplete();
    stm32wle5_aes::read(output);
    toBe.setFromHexString("a9dcf5aa138056e259e7be57958e72d8");
    TEST_ASSERT_TRUE(output == toBe);

    // third block

    stm32wle5_aes::write(input);
    while (!stm32wle5_aes::isComputationComplete()) {
    }
    stm32wle5_aes::clearComputationComplete();
    stm32wle5_aes::read(output);
    toBe.setFromHexString("626caecce6b25a25524cb32b7ec1374e");
    TEST_ASSERT_TRUE(output == toBe);
}

int main(int argc, char **argv) {
    HAL_Init();
    HAL_Delay(2000);        // required for testing framework to connect
    SystemClock_Config();

    UNITY_BEGIN();
    RUN_TEST(test_initialize);
    RUN_TEST(test_enable_disable);
    RUN_TEST(test_setKey);
    RUN_TEST(test_setInitVector);
    RUN_TEST(test_hwEncryptSingleBlock);
    RUN_TEST(test_hwEncryptLoRaWANPayload);
    RUN_TEST(test_hwCalculateMic);
    UNITY_END();
}

void SysTick_Handler(void) {
    HAL_IncTick();
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
    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.LSEState            = RCC_LSE_ON;
    RCC_OscInitStruct.MSIState            = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.MSIClockRange       = RCC_MSIRANGE_8;
    RCC_OscInitStruct.LSIDiv              = RCC_LSI_DIV1;
    RCC_OscInitStruct.LSIState            = RCC_LSI_ON;
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

void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}
