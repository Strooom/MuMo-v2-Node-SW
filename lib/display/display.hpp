// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>



enum class displayRotation : uint32_t {        // rotation is clockwise
    rotation0,
    rotation90,
    rotation180,
    rotation270
};

enum class displayMirroring : uint32_t {
    none,
    horizontal,
    vertical,
    both
};

enum class displayPresence : uint32_t {
    unknown,
    present,
    notPresent
};

class display {
  public:
    display() = delete;
    static void detectPresence();
    static bool isPresent();
    static void wakeUp();
    static bool isReady();
    static void update();
    static void goSleep();

    static void setPixel(uint32_t x, uint32_t y);
    static void clearPixel(uint32_t x, uint32_t y);
    static void clearAllPixels();
    static bool getPixel(uint32_t x, uint32_t y);

    static constexpr uint32_t widthInPixels{200};                     // [pixels]
    static constexpr uint32_t heightInPixels{200};                    // [pixels]
    static constexpr uint32_t widthInBytes{widthInPixels / 8};        // [bytes]

    static void dump();

#ifndef unitTesting

  private:
#endif

    static displayPresence displayPresent;
    static constexpr uint32_t bufferSize{widthInPixels * heightInPixels / 8};        // 1 bit per pixel
    static uint8_t displayBuffer[bufferSize];

    enum class SSD1681Commands : uint8_t {
        DRIVER_OUTPUT_CONTROL                = 0x01,
        BOOSTER_SOFT_START_CONTROL           = 0x0C,
        GATE_SCAN_START_POSITION             = 0x0F,
        DEEP_SLEEP_MODE                      = 0x10,
        DATA_ENTRY_MODE_SETTING              = 0x11,
        SW_RESET                             = 0x12,
        TEMPERATURE_SENSOR_SELECTION         = 0x18,
        MASTER_ACTIVATION                    = 0x20,
        DISPLAY_UPDATE_CONTROL_1             = 0x21,
        DISPLAY_UPDATE_CONTROL_2             = 0x22,
        WRITE_RAM                            = 0x24,
        WRITE_RAM_2                          = 0x26,        // undocumented, reverse engineered from example code
        WRITE_VCOM_REGISTER                  = 0x2C,
        WRITE_LUT_REGISTER                   = 0x32,
        SET_DUMMY_LINE_PERIOD                = 0x3A,
        SET_GATE_TIME                        = 0x3B,
        BORDER_WAVEFORM_CONTROL              = 0x3C,
        SET_RAM_X_ADDRESS_START_END_POSITION = 0x44,
        SET_RAM_Y_ADDRESS_START_END_POSITION = 0x45,
        SET_RAM_X_ADDRESS_COUNTER            = 0x4E,
        SET_RAM_Y_ADDRESS_COUNTER            = 0x4F,
        TERMINATE_FRAME_READ_WRITE           = 0xFF,
    };

    static displayRotation rotation;
    static displayMirroring mirroring;

    static void hardwareReset();                      // drive display-reset line down for 10ms
    static void softwareReset();                      // send software reset command to display
    static void setDataOrCommand(bool isData);        // controls the D/C line
    static void selectChip(bool active);              // controls the CS line
    static bool isBusy();                             // checks the display busy line

    static void writeCommand(SSD1681Commands theCommand, uint8_t *data, uint32_t length);        // write a command with optionally some data parameters
    static void writeData(uint8_t *data, uint32_t length);                                       //
    static void writeData(uint8_t data);                                                         //
    static void write(uint8_t *data, uint32_t length);                                           // write an array of bytes to the display
    static void write(uint8_t data);                                                             // write a single byte to the display

    static void waitWhileBusy();

    static void rotateCoordinates(uint32_t &x, uint32_t &y);
    static void mirrorCoordinates(uint32_t &x, uint32_t &y);
    static void rotateAndMirrorCoordinates(uint32_t &x, uint32_t &y);
    static bool isInBounds(uint32_t x, uint32_t y) { return ((x < display::widthInPixels) && (y < display::heightInPixels)); };
    static void swapCoordinates(uint32_t &c1, uint32_t &c2);
    static void mirrorCoordinate(uint32_t &c, uint32_t maxC) { c = (maxC - 1) - c; };
    static uint32_t getByteOffset(uint32_t x, uint32_t y) { return ((y * widthInBytes) + (x / 8)); };
    static uint32_t getBitOffset(uint32_t x) { return (7 - (x % 8)); };
};
