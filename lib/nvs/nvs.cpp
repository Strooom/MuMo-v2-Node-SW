// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include <nvs.hpp>
#include <i2c.hpp>

#ifndef generic
#include "main.h"
extern I2C_HandleTypeDef hi2c2;
#else
#include <cstring>
uint32_t nonVolatileStorage::mockEepromNmbr64KPages{2};
uint8_t nonVolatileStorage::mockEepromMemory[maxNmbr64KBanks * 64 * 1024];
#endif

uint32_t nonVolatileStorage::nmbr64KBanks{0};

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
    uint8_t data[pageSize];
    const uint32_t nmbrOfPages{totalSize() / pageSize};

    for (uint32_t i = 0; i < pageSize; i++) {
        data[i] = value;
    }

    for (uint32_t i = 0; i < nmbrOfPages; i++) {
        write(i * pageSize, data, pageSize);
    }
}

uint32_t nonVolatileStorage::detectNmbr64KBanks() {
#ifndef generic
    nmbr64KBanks = 0;
    for (uint8_t blockIndex = 0; blockIndex < maxNmbr64KBanks; blockIndex++) {
        if (HAL_OK == HAL_I2C_IsDeviceReady(&hi2c2, static_cast<uint16_t>((baseI2cAddress + blockIndex) << 1), halNmbrOfTrials, halTimeoutInMs)) {
            nmbr64KBanks++;
        } else {
            return nmbr64KBanks;
        }
    }
#else
    nmbr64KBanks = mockEepromNmbr64KPages;
#endif
    return nmbr64KBanks;
}

// Reading and Writing a single byte. Simple but not efficient when you need to read or write a range of bytes

uint8_t nonVolatileStorage::read(const uint32_t address) {
#ifndef generic
    uint8_t result;
    bool i2cState = i2c::isActive();
    if (i2cState != i2c::isAwake) {
        i2c::wakeUp();
    }
    HAL_I2C_Mem_Read(&hi2c2, static_cast<uint16_t>(bankI2cAddress(address) << 1), bankOffset(address), I2C_MEMADD_SIZE_16BIT, &result, 1U, halTimeoutInMs);
    if (i2cState == i2c::wasSleeping) {
        i2c::goSleep();
    }
    return result;
#else
    return mockEepromMemory[address];
#endif
}

void nonVolatileStorage::write(const uint32_t address, const uint8_t data) {
    uint8_t dataCopy{data};
    bool i2cState = i2c::isActive();
    if (i2cState != i2c::isAwake) {
        i2c::wakeUp();
    }
    disableWriteProtect();
#ifndef generic
    HAL_I2C_Mem_Write(&hi2c2, static_cast<uint16_t>(bankI2cAddress(address) << 1), bankOffset(address), I2C_MEMADD_SIZE_16BIT, &dataCopy, 1U, halTimeoutInMs);
    HAL_Delay(writeCycleBusyTimeInMs);        // wait for the EEPROM to internally finish writing
#else
    mockEepromMemory[address] = data;
#endif
    enableWriteProtect();
    if (i2cState == i2c::wasSleeping) {
        i2c::goSleep();
    }
}

// Reading or Writing a range of bytes but all inside a single page

void nonVolatileStorage::readInPage(const uint32_t startAddress, uint8_t* destination, const uint32_t dataLength) {
#ifndef generic
    bool i2cState = i2c::isActive();
    if (i2cState != i2c::isAwake) {
        i2c::wakeUp();
    }
    HAL_I2C_Mem_Read(&hi2c2, static_cast<uint16_t>(bankI2cAddress(startAddress) << 1), bankOffset(startAddress), I2C_MEMADD_SIZE_16BIT, destination, static_cast<uint16_t>(dataLength), halTimeoutInMs);
    if (i2cState == i2c::wasSleeping) {
        i2c::goSleep();
    }
#else
    (void)memcpy(destination, mockEepromMemory + startAddress, dataLength);
#endif
}

void nonVolatileStorage::writeInPage(const uint32_t startAddress, const uint8_t* source, const uint32_t dataLength) {
    bool i2cState = i2c::isActive();
    if (i2cState != i2c::isAwake) {
        i2c::wakeUp();
    }
    disableWriteProtect();
#ifndef generic
    uint8_t* data{const_cast<uint8_t*>(source)};
    HAL_I2C_Mem_Write(&hi2c2, static_cast<uint16_t>(bankI2cAddress(startAddress) << 1), bankOffset(startAddress), I2C_MEMADD_SIZE_16BIT, data, static_cast<uint16_t>(dataLength), halTimeoutInMs);
    HAL_Delay(writeCycleBusyTimeInMs);        // wait for the EEPROM to finish writing
#else
    (void)memcpy(mockEepromMemory + startAddress, source, dataLength);
#endif
    enableWriteProtect();
    if (i2cState == i2c::wasSleeping) {
        i2c::goSleep();
    }
}

// Reading and Writing a range of bytes. More complex because the range may span several pages and the HW can only read or write to a single page
// We also apply it for reading, as a read may span multiple 64Kbanks

void nonVolatileStorage::read(const uint32_t startAddress, uint8_t* destination, const uint32_t dataLength) {
    uint8_t* remainingData{destination};
    uint32_t remainingLength{dataLength};
    uint32_t currentAddress{startAddress};
    bool i2cState = i2c::isActive();
    if (i2cState != i2c::isAwake) {
        i2c::wakeUp();
    }
    while (remainingLength > 0) {
        uint32_t bytesInThisPage = bytesInCurrentPage(currentAddress, remainingLength);
        readInPage(currentAddress, remainingData, bytesInThisPage);
        currentAddress += bytesInThisPage;
        remainingData += bytesInThisPage;
        remainingLength -= bytesInThisPage;
    }
    if (i2cState == i2c::wasSleeping) {
        i2c::goSleep();
    }
}

void nonVolatileStorage::write(const uint32_t startAddress, const uint8_t* source, const uint32_t dataLength) {
    const uint8_t* remainingData{source};
    uint32_t remainingLength{dataLength};
    uint32_t currentAddress{startAddress};
    bool i2cState = i2c::isActive();
    if (i2cState != i2c::isAwake) {
        i2c::wakeUp();
    }
    while (remainingLength > 0) {
        uint32_t bytesInThisPage = bytesInCurrentPage(currentAddress, remainingLength);
        writeInPage(currentAddress, remainingData, bytesInThisPage);
        currentAddress += bytesInThisPage;
        remainingData += bytesInThisPage;
        remainingLength -= bytesInThisPage;
    }
    if (i2cState == i2c::wasSleeping) {
        i2c::goSleep();
    }
}

void nonVolatileStorage::disableWriteProtect() {
#ifndef generic
    bool i2cState = i2c::isActive();
    if (i2cState != i2c::isAwake) {
        i2c::wakeUp();
    }
    HAL_GPIO_WritePin(GPIOB, writeProtect_Pin, GPIO_PIN_RESET);        // Drive writeProtect LOW = enable write = disable write-protect
    if (i2cState == i2c::wasSleeping) {
        i2c::goSleep();
    }
#endif
}

void nonVolatileStorage::enableWriteProtect() {
#ifndef generic
    bool i2cState = i2c::isActive();
    if (i2cState != i2c::isAwake) {
        i2c::wakeUp();
    }
    HAL_GPIO_WritePin(GPIOB, writeProtect_Pin, GPIO_PIN_SET);        // Drive writeProtect HIGH = disable write = enable write-protect
    if (i2cState == i2c::wasSleeping) {
        i2c::goSleep();
    }
#endif
}

uint32_t nonVolatileStorage::getMeasurementsAreaSize() {
    if (totalSize() > 0) {
        return totalSize() - settingsSize;
    } else {
        return 0;
    }
}

