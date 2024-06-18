// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include "nvs.hpp"
#include <i2c.hpp>

#ifndef generic
#include "main.h"
extern I2C_HandleTypeDef hi2c2;
#else
uint8_t mockEepromMemory[nonVolatileStorage::totalSize];
#include <cstring>
#endif

uint32_t nonVolatileStorage::bytesInCurrentPage(uint32_t address, uint32_t dataLength) {
    uint32_t startPage{pageNumber(address)};
    uint32_t endPage{pageNumber(address + dataLength)};

    if (endPage == startPage) {
        return dataLength;
    } else {
        return pageSize - (address % pageSize);
    }
}

void nonVolatileStorage::erase() {
    fill(0xFF);
}

void nonVolatileStorage::fill(uint8_t value) {
    uint8_t data[128];        // writing data in chunks of 128 bytes as this is optimized for the EEPROM device TODO : make this work for different EEPROM sizes / page sizes
    constexpr uint32_t pageSize{128};
    constexpr uint32_t nmbrOfPages{totalSize / pageSize};

    for (uint32_t i = 0; i < pageSize; i++) {
        data[i] = value;
    }

    for (uint32_t i = 0; i < nmbrOfPages; i++) {
        write(i * pageSize, data, pageSize);
    }
}

bool nonVolatileStorage::isPresent() {
    bool i2cState = i2c::isAwake();
    if (!i2cState) {
        i2c::wakeUp();
    }
#ifndef generic
    if (HAL_OK != HAL_I2C_IsDeviceReady(&hi2c2, i2cAddress << 1, halTrialsIsPresent, halTimeoutIsPresent)) {        // testing presence of the first bank of 64K (U7)
        return false;
    }
    if (HAL_OK != HAL_I2C_IsDeviceReady(&hi2c2, (i2cAddress << 1) + 1, halTrialsIsPresent, halTimeoutIsPresent)) {        // testing presence of the second bank of 64K (U8)
        return false;
    }
    if (!i2cState) {
        i2c::goSleep();
    }
#endif
    return true;
}

void nonVolatileStorage::read(const uint32_t startAddress, uint8_t* data, const uint32_t dataLength) {
    bool i2cState = i2c::isAwake();
    if (!i2cState) {
        i2c::wakeUp();
    }
#ifndef generic
    HAL_I2C_Mem_Read(&hi2c2, i2cAddress << 1, startAddress, I2C_MEMADD_SIZE_16BIT, data, dataLength, halTimeoutIsPresent);        //
#else
    (void)memcpy(data, mockEepromMemory + startAddress, dataLength);
#endif
    if (!i2cState) {
        i2c::goSleep();
    }
}

void nonVolatileStorage::write(const uint32_t startAddress, const uint8_t* data, const uint32_t dataLength) {
    // This function can write accross eeprom page boundaries
    uint8_t* remainingData{const_cast<uint8_t*>(data)};
    uint32_t remainingLength{dataLength};
    uint32_t currentAddress{startAddress};
    bool i2cState = i2c::isAwake();
    if (!i2cState) {
        i2c::wakeUp();
    }
#ifndef generic
    HAL_GPIO_WritePin(GPIOB, writeProtect_Pin, GPIO_PIN_RESET);        // Drive writeProtect LOW = enable write
#endif
    while (remainingLength > 0) {
        uint32_t bytesInThisPage = bytesInCurrentPage(currentAddress, remainingLength);
#ifndef generic
        HAL_I2C_Mem_Write(&hi2c2, i2cAddress << 1, currentAddress, I2C_MEMADD_SIZE_16BIT, remainingData, bytesInThisPage, halTimeoutIsPresent);        // my wrapper does not allow the to-be-written source data to be modified, but the STM32 HAL doesn't have a const uint8_t ptr
        HAL_Delay(writeCycleTime);                                                                                                                     // wait for the EEPROM to finish writing
#else
        (void)memcpy(mockEepromMemory + currentAddress, remainingData, bytesInThisPage);
#endif

        currentAddress += bytesInThisPage;
        remainingData += bytesInThisPage;
        remainingLength -= bytesInThisPage;
    }
#ifndef generic
    HAL_GPIO_WritePin(GPIOB, writeProtect_Pin, GPIO_PIN_SET);        // disable write
#endif
    if (!i2cState) {
        i2c::goSleep();
    }

}
