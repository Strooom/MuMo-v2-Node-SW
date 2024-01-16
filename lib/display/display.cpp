// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include "display.hpp"

#ifndef generic
#include "main.h"
extern SPI_HandleTypeDef hspi2;
#endif

// extern peripheralRegister PORTA_BSRR;
// extern peripheralRegister PORTB_BSRR;
// extern peripheralRegister PORTC_BSRR;
// extern peripheralRegister PORTB_IDR;

displayPresence display::displayPresent{displayPresence::unknown};
displayRotation display::rotation{displayRotation::rotation270};
displayMirroring display::mirroring{displayMirroring::none};
uint8_t display::displayBuffer[display::bufferSize];

bool display::isPresent() {
    if (displayPresent == displayPresence::unknown) {
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
    return (displayPresent == displayPresence::present);
}

void display::initialize() {
    hardwareReset();
    waitWhileBusy();
    softwareReset();
    waitWhileBusy();

    uint8_t commandData[4]{0};

    commandData[0] = 0xC7;        // this seems to be (height - 1) % 256
    commandData[1] = 0x00;        // this seems to be (height - 1) / 256
    commandData[2] = 0x00;
    writeCommand(SSD1681Commands::DRIVER_OUTPUT_CONTROL, commandData, 3);
    waitWhileBusy();

    commandData[0] = 0x11;        // from example - don't know what this does
    writeCommand(SSD1681Commands::DATA_ENTRY_MODE_SETTING, commandData, 1);
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

    commandData[0] = 0x05;
    writeCommand(SSD1681Commands::BORDER_WAVEFORM_CONTROL, commandData, 1);
    waitWhileBusy();

    commandData[0] = 0x80;        // Selects internal temperature sensor
    writeCommand(SSD1681Commands::TEMPERATURE_SENSOR_SELECTION, commandData, 1);
    waitWhileBusy();

    commandData[0] = 0x0;
    writeCommand(SSD1681Commands::SET_RAM_X_ADDRESS_COUNTER, commandData, 1);
    waitWhileBusy();

    commandData[0] = 0xC7;        // this seems to be (height - 1) % 256
    commandData[1] = 0x00;        // this seems to be (height - 1) / 256
    writeCommand(SSD1681Commands::SET_RAM_Y_ADDRESS_COUNTER, commandData, 2);
    waitWhileBusy();

    clearAllPixels();
}

void display::run() {
}

bool display::isReady() {
    // TODO : implement : when updating is done, ie. busyflag no longer set, return true
    return true;
}

void display::goSleep() {
    uint8_t commandData[1]{0x01};
    writeCommand(SSD1681Commands::DEEP_SLEEP_MODE, commandData, 1);
}

void display::setPixel(uint32_t x, uint32_t y) {
    if (isInBounds(x, y)) {
        rotateAndMirrorCoordinates(x, y);
        uint32_t byteOffset       = getByteOffset(x, y);
        uint32_t bitOffset        = getBitOffset(x);
        displayBuffer[byteOffset] = displayBuffer[byteOffset] & ~(1 << bitOffset);
    }
}

void display::clearPixel(uint32_t x, uint32_t y) {
    if (isInBounds(x, y)) {
        rotateAndMirrorCoordinates(x, y);
        uint32_t byteOffset       = getByteOffset(x, y);
        uint32_t bitOffset        = getBitOffset(x);
        displayBuffer[byteOffset] = displayBuffer[byteOffset] | (1 << bitOffset);
    }
}

void display::clearAllPixels() {
    for (uint32_t i = 0; i < bufferSize; i++) {
        displayBuffer[i] = 0xFF;
    }
}

bool display::getPixel(uint32_t x, uint32_t y) {
    if (isInBounds(x, y)) {
        rotateAndMirrorCoordinates(x, y);
        uint32_t byteOffset = getByteOffset(x, y);
        uint32_t bitOffset  = getBitOffset(x);
        return ((displayBuffer[byteOffset] & (1 << bitOffset)) == 0);
    } else {
        return false;
    }
}

// void display::changePixel(uint32_t x, uint32_t y, bool onOff) {
//     if (!isInBounds(x, y)) {        // NOTE : as our display is 200 x 200, we could already perform this check earlier (eg in graphics::drawPixel), and avoid executing some code in case the pixel is out of bounds
//         return;
//     }
//     rotateAndMirrorCoordinates(x, y);
//     if (onOff) {
//         setPixel(x, y);
//     } else {
//         clearPixel(x, y);
//     }
// }

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
        case displayRotation::rotation0:
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
        case displayMirroring::none:
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
    // PORTA_BSRR.write(1 << 0);               // reset = LOW
    HAL_Delay(10U);        // datasheet, section 4.2
    HAL_GPIO_WritePin(GPIOA, displayReset_Pin, GPIO_PIN_SET);
    // PORTA_BSRR.write(1 << (0 + 16));        // reset = HIGH
    HAL_Delay(10U);        //
#endif
}

void display::softwareReset() {
    writeCommand(SSD1681Commands::SW_RESET, nullptr, 0);
#ifndef generic
    HAL_Delay(10U);        // datasheet, section 4.2
#endif
}

void display::setDataOrCommand(bool isData) {
#ifndef generic
    if (isData) {
        HAL_GPIO_WritePin(GPIOB, displayDataCommand_Pin, GPIO_PIN_SET);
        // PORTB_BSRR.write(1 << 14);               // data = HIGH
    } else {        //
        HAL_GPIO_WritePin(GPIOB, displayDataCommand_Pin, GPIO_PIN_RESET);
        // PORTB_BSRR.write(1 << (14 + 16));        // command = LOW
    }
#endif
}

void display::selectChip(bool active) {
#ifndef generic
    if (active) {
        HAL_GPIO_WritePin(GPIOB, displayChipSelect_Pin, GPIO_PIN_RESET);
        // PORTB_BSRR.write(1 << (5 + 16));        // active = LOW
    } else {        //
        HAL_GPIO_WritePin(GPIOB, displayChipSelect_Pin, GPIO_PIN_SET);
        // PORTB_BSRR.write(1 << 5);               // release = HIGH
    }
#endif
}

bool display::isBusy() {
#ifndef generic
    return (GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOB, displayBusy_Pin));
// return (PORTB_IDR.readBit(10));
#else
    return false;
#endif
}

void display::write(uint8_t* data, uint32_t length) {
    selectChip(true);
#ifndef generic
    HAL_SPI_Transmit(&hspi2, data, length, 1000);        // TODO : get the HAL timeout stuff right
#endif
    selectChip(false);
}

void display::write(uint8_t data) {
    write(&data, 1);
}

void display::writeData(uint8_t* data, uint32_t length) {
    setDataOrCommand(true);
    write(data, length);
}

void display::writeData(uint8_t data) {
    writeData(&data, 1);
}

void display::writeCommand(SSD1681Commands theCommand, uint8_t* theData, uint32_t dataLength) {
    selectChip(true);
    setDataOrCommand(false);
    uint8_t commandAsByte = static_cast<uint8_t>(theCommand);
#ifndef generic
    HAL_SPI_Transmit(&hspi2, &commandAsByte, 1U, 1U);
#endif
    if (dataLength > 0) {
        setDataOrCommand(true);
#ifndef generic
        HAL_SPI_Transmit(&hspi2, theData, dataLength, 1U);
#endif
    }
    selectChip(false);
}

void display::waitWhileBusy() {
    // TODO : this is potentially and endless loop -> add a timeout
    while (isBusy()) {
        asm("NOP");
    }
}



void display::update() {
    uint8_t commandData[4]{0};
    writeCommand(SSD1681Commands::WRITE_RAM, nullptr, 0);
    writeData(displayBuffer, bufferSize);
    commandData[0] = 0xF7; // Full update
    writeCommand(SSD1681Commands::DISPLAY_UPDATE_CONTROL_2, commandData, 1);
    writeCommand(SSD1681Commands::MASTER_ACTIVATION, nullptr, 0);
    waitWhileBusy();
}

