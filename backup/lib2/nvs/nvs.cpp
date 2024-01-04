// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include "nvs.h"

#ifndef generic
#include "main.h"
extern I2C_HandleTypeDef hi2c2;
#else
#include <cstring>        // required for memcpy
static uint8_t memory[nonVolatileStorage::size];        // array emulating the EEPROM for unitTesting
#endif

void nonVolatileStorage::erase() {
    fill(0xFF);
}

void nonVolatileStorage::fill(uint8_t value) {
    uint8_t data[128];        // writing data in chunks of 128 bytes as this is optimized for the EEPROM device
    constexpr uint32_t pageSize{128};
    constexpr uint32_t nmbrOfPages{size / pageSize};

    for (uint32_t i = 0; i < pageSize; i++) {
        data[i] = value;
    }

    for (uint32_t i = 0; i < nmbrOfPages; i++) {
        write(i * pageSize, data, pageSize);
    }
}

bool nonVolatileStorage::isPresent() {
#ifndef generic
    if (HAL_OK != HAL_I2C_IsDeviceReady(&hi2c2, i2cAddress << 1, halTrials, halTimeout)) {        // testing presence of the first bank of 64K (U7)
        return false;
    }
    if (HAL_OK != HAL_I2C_IsDeviceReady(&hi2c2, (i2cAddress << 1) + 1, halTrials, halTimeout)) {        // testing presence of the second bank of 64K (U8)
        return false;
    }
#endif
    return true;
}

void nonVolatileStorage::read(const uint32_t startAddress, uint8_t* data, const uint32_t dataLength) {
#ifndef generic
    HAL_I2C_Mem_Read(&hi2c2, i2cAddress << 1, startAddress, I2C_MEMADD_SIZE_16BIT, data, dataLength, halTimeout);        //
#else
    memcpy(data, memory + startAddress, dataLength);
#endif
}

void nonVolatileStorage::write(const uint32_t startAddress, const uint8_t* data, const uint32_t dataLength) {
#ifndef generic
    HAL_GPIO_WritePin(GPIOB, writeProtect_Pin, GPIO_PIN_RESET);                                                                                 // Drive writeProtect LOW = enable write
    HAL_I2C_Mem_Write(&hi2c2, i2cAddress << 1, startAddress, I2C_MEMADD_SIZE_16BIT, const_cast<uint8_t*>(data), dataLength, halTimeout);        // my wrapper does not allow the to-be-written source data to be modified, but the STM32 HAL doesn't have a const uint8_t ptr
    HAL_Delay(writeCycleTime);                                                                                                                  // the EEPROM needs 3.5 ms to internally write the data, if WriteProtect goes HIGH too early, the data is not written
    HAL_GPIO_WritePin(GPIOB, writeProtect_Pin, GPIO_PIN_SET);                                                                                   // disable write
#else
    memcpy(memory + startAddress, data, dataLength);
#endif
}
