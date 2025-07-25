// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include <display.hpp>
#include <cstring>
#include <spi.hpp>

#ifndef generic
#include "main.h"
extern SPI_HandleTypeDef hspi2;
#else
bool display::mockDisplayPresent{true};
#endif

displayPresence display::displayPresent{displayPresence::unknown};
displayRotation display::rotation{displayRotation::rotation270};
displayMirroring display::mirroring{displayMirroring::none};
uint8_t display::newFrameBuffer[display::bufferSize];
uint8_t display::oldFrameBuffer[display::bufferSize];
uint32_t display::refreshCounter{};

void display::detectPresence() {
    hardwareReset();
    goSleep();
#ifndef generic
    HAL_Delay(1U);
#endif
    if (isBusy()) {
        displayPresent = displayPresence::present;
    } else {
        displayPresent = displayPresence::notPresent;
    }
}

bool display::isPresent() {
#ifdef generic
    if (mockDisplayPresent) {
        displayPresent = displayPresence::present;
    } else {
        displayPresent = displayPresence::notPresent;
    }
#else
    if (displayPresent == displayPresence::unknown) {
        detectPresence();
    }
#endif
    return (displayPresent == displayPresence::present);
}

void display::goSleep() {
    uint8_t commandData[1]{0x03};        // Deep Sleep Mode 2 - SSD1681 Datasheet Rev 0l.13 Page 23
    writeCommand(SSD1681Commands::DEEP_SLEEP_MODE, commandData, 1);
}

void display::setPixel(uint32_t x, uint32_t y) {
    if (isInBounds(x, y)) {
        rotateAndMirrorCoordinates(x, y);
        uint32_t byteOffset = getByteOffset(x, y);
        uint32_t bitOffset  = getBitOffset(x);
        newFrameBuffer[byteOffset] &= ~(static_cast<uint8_t>(1 << bitOffset));
    }
}

void display::clearPixel(uint32_t x, uint32_t y) {
    if (isInBounds(x, y)) {
        rotateAndMirrorCoordinates(x, y);
        uint32_t byteOffset = getByteOffset(x, y);
        uint32_t bitOffset  = getBitOffset(x);
        newFrameBuffer[byteOffset] |= static_cast<uint8_t>(1 << bitOffset);
    }
}

void display::clearAllPixels() {
    memset(newFrameBuffer, 0xFF, bufferSize);
}

bool display::getPixel(uint32_t x, uint32_t y) {
    if (isInBounds(x, y)) {
        rotateAndMirrorCoordinates(x, y);
        uint32_t byteOffset = getByteOffset(x, y);
        uint32_t bitOffset  = getBitOffset(x);
        return ((newFrameBuffer[byteOffset] & (1 << bitOffset)) == 0);
    } else {
        return false;
    }
}

void display::swapCoordinates(uint32_t& c1, uint32_t& c2) {
    uint32_t temp = c1;
    c1            = c2;
    c2            = temp;
}

void display::rotateCoordinates(uint32_t& x, uint32_t& y) {
    switch (display::rotation) {
        case displayRotation::rotation90:
            swapCoordinates(x, y);
            mirrorCoordinate(y, display::heightInPixels);
            break;
        case displayRotation::rotation180:
            mirrorCoordinate(x, display::widthInPixels);
            mirrorCoordinate(y, display::heightInPixels);
            break;
        case displayRotation::rotation270:
            swapCoordinates(x, y);
            mirrorCoordinate(x, display::widthInPixels);
            break;
        default:
            break;
    }
}

void display::mirrorCoordinates(uint32_t& x, uint32_t& y) {
    switch (display::mirroring) {
        case displayMirroring::horizontal:
            mirrorCoordinate(x, display::widthInPixels);
            break;
        case displayMirroring::vertical:
            mirrorCoordinate(y, display::heightInPixels);
            break;
        case displayMirroring::both:
            mirrorCoordinate(x, display::widthInPixels);
            mirrorCoordinate(y, display::heightInPixels);
            break;
        default:
            break;
    }
}

void display::rotateAndMirrorCoordinates(uint32_t& x, uint32_t& y) {
    rotateCoordinates(x, y);
    mirrorCoordinates(x, y);
}

void display::hardwareReset() {
#ifndef generic
    HAL_GPIO_WritePin(GPIOA, displayReset_Pin, GPIO_PIN_RESET);
    HAL_Delay(10U);        // SSD1681 Datasheet Rev 0l.13, section 4.2
    HAL_GPIO_WritePin(GPIOA, displayReset_Pin, GPIO_PIN_SET);
    HAL_Delay(10U);        //
#endif
}

void display::softwareReset() {
    writeCommand(SSD1681Commands::SW_RESET, nullptr, 0);
#ifndef generic
    HAL_Delay(10U);        // SSD1681 Datasheet Rev 0l.13, section 4.2
#endif
}

void display::setDataOrCommand(const bool isData) {
#ifndef generic
    if (isData) {
        HAL_GPIO_WritePin(GPIOB, displayDataCommand_Pin, GPIO_PIN_SET);
    } else {        //
        HAL_GPIO_WritePin(GPIOB, displayDataCommand_Pin, GPIO_PIN_RESET);
    }
#endif
}

void display::selectChip(const bool active) {
#ifndef generic
    if (active) {
        HAL_GPIO_WritePin(GPIOB, displayChipSelect_Pin, GPIO_PIN_RESET);
    } else {        //
        HAL_GPIO_WritePin(GPIOB, displayChipSelect_Pin, GPIO_PIN_SET);
    }
#endif
}

bool display::isReady() {
    return !isBusy();
}

bool display::isBusy() {
#ifndef generic
    return (GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOB, displayBusy_Pin));
#else
    return false;
#endif
}

void display::write(uint8_t* data, const uint32_t length) {
    selectChip(true);
#ifndef generic
    HAL_SPI_Transmit(&hspi2, data, static_cast<const uint16_t>(length), spi::spiDisplayTimeout);
#endif
    selectChip(false);
}

void display::write(uint8_t data) {
    write(&data, 1);
}

void display::writeData(uint8_t* data, const uint32_t length) {
    setDataOrCommand(true);
    write(data, length);
}

void display::writeData(uint8_t data) {
    writeData(&data, 1);
}

void display::writeCommand(const SSD1681Commands theCommand, uint8_t* theData, const uint32_t dataLength) {
    selectChip(true);
    setDataOrCommand(false);
#ifndef generic
    uint8_t commandAsByte = static_cast<uint8_t>(theCommand);
    HAL_SPI_Transmit(&hspi2, &commandAsByte, 1U, 1U);
#endif
    if (dataLength > 0) {
        setDataOrCommand(true);
#ifndef generic
        HAL_SPI_Transmit(&hspi2, theData, static_cast<uint16_t>(dataLength), 1U);
#endif
    }
    selectChip(false);
}

void display::waitWhileBusy() {
#ifndef generic
    while (isBusy()) {
        asm("NOP");
    }
#endif
}

void display::update(bool full) {
    hardwareReset();
    waitWhileBusy();
    softwareReset();
    waitWhileBusy();

    uint8_t commandData[4]{0};

    // DRIVER_OUTPUT_CONTROL : uses PowerOnReset defaults
    // DATA_ENTRY_MODE_SETTING : uses PowerOnReset defaults
    // BORDER_WAVEFORM_CONTROL : uses PowerOnReset defaults : // commandData[0] = 0x05; is also working

    commandData[0] = 0x80;        // Selects internal temperature sensor - required as this is not the powerOn default
    writeCommand(SSD1681Commands::TEMPERATURE_SENSOR_SELECTION, commandData, 1);
    waitWhileBusy();

    commandData[0] = 0x00;
    commandData[1] = 0x18;        // this seems to be ((widthInPixels / 8) - 1)
    writeCommand(SSD1681Commands::SET_RAM_X_ADDRESS_START_END_POSITION, commandData, 2);
    waitWhileBusy();

    commandData[0] = 0xC7;        // this seems to be (height - 1) % 256
    commandData[1] = 0x00;        // this seems to be (height - 1) / 256
    commandData[2] = 0x00;
    commandData[3] = 0x00;
    writeCommand(SSD1681Commands::SET_RAM_Y_ADDRESS_START_END_POSITION, commandData, 4);
    waitWhileBusy();

    // SET_RAM_X_ADDRESS_COUNTER : uses PowerOnReset defaults

    commandData[0] = 0xC7;        // this seems to be (height - 1) % 256
    commandData[1] = 0x00;        // this seems to be (height - 1) / 256
    writeCommand(SSD1681Commands::SET_RAM_Y_ADDRESS_COUNTER, commandData, 2);
    waitWhileBusy();

    writeCommand(SSD1681Commands::WRITE_RAM_0, nullptr, 0);
    writeData(newFrameBuffer, bufferSize);
    waitWhileBusy();

    if (full) {
        commandData[0] = 0xF7;        // SSD1681 Datasheet Rev 0l.13 - Full update Display Mode 1
    } else {
        writeCommand(SSD1681Commands::WRITE_RAM_1, nullptr, 0);        // For 'partial' updates, we write the old picture to RAM1
        writeData(oldFrameBuffer, bufferSize);
        waitWhileBusy();
        commandData[0] = 0xFF;        // SSD1681 Datasheet Rev 0l.13 - Partial update Display Mode 1
    }

    writeCommand(SSD1681Commands::DISPLAY_UPDATE_CONTROL_2, commandData, 1);
    writeCommand(SSD1681Commands::MASTER_ACTIVATION, nullptr, 0);
    copyNewToOldFrameBuffer();
    waitWhileBusy();
    goSleep();
}

void display::update() {
    update(refreshCounter == 0);
    refreshCounter = (refreshCounter + 1) % fullRefreshCount;
}
