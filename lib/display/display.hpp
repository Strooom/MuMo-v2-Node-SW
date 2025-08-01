// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include <cstring>

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
    static bool isReady();
    static void update(bool full);        // manual : full = true for full refresh, false for fast refresh
    static void update();                 // automatic = fast with full refresh every 8 updates
    static void goSleep();

    static void setPixel(uint32_t x, uint32_t y);
    static void clearPixel(uint32_t x, uint32_t y);
    static void clearAllPixels();
    static bool getPixel(uint32_t x, uint32_t y);

    static constexpr uint32_t widthInPixels{200};                     // [pixels]
    static constexpr uint32_t heightInPixels{200};                    // [pixels]
    static constexpr uint32_t widthInBytes{widthInPixels / 8};        // [bytes]

#ifndef unitTesting

  private:
#endif

    static displayPresence displayPresent;
    static constexpr uint32_t bufferSize{widthInPixels * heightInPixels / 8};        // 1 bit per pixel
    alignas(4) static uint8_t newFrameBuffer[bufferSize];
    alignas(4) static uint8_t oldFrameBuffer[bufferSize];
    static uint32_t refreshCounter;
    static constexpr uint32_t fullRefreshCount{8};

    enum class SSD1681Commands : uint8_t {
        DRIVER_OUTPUT_CONTROL                = 0x01,
        BOOSTER_SOFT_START_CONTROL           = 0x0C,
        GATE_SCAN_START_POSITION             = 0x0F,
        DEEP_SLEEP_MODE                      = 0x10,
        DATA_ENTRY_MODE_SETTING              = 0x11,
        SW_RESET                             = 0x12,
        TEMPERATURE_SENSOR_SELECTION         = 0x18,
        WriteTemperatureRegister             = 0x1A,
        MASTER_ACTIVATION                    = 0x20,
        DISPLAY_UPDATE_CONTROL_1             = 0x21,
        DISPLAY_UPDATE_CONTROL_2             = 0x22,
        WRITE_RAM_0                          = 0x24,
        WRITE_RAM_1                          = 0x26,        // undocumented, reverse engineered from example code
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

    static void hardwareReset();
    static void softwareReset();
    static void setDataOrCommand(const bool isData);
    static void selectChip(const bool active);
    static bool isBusy();

    static void writeCommand(const SSD1681Commands theCommand, uint8_t *data, const uint32_t length);
    static void writeData(uint8_t *data, const uint32_t length);
    static void writeData(uint8_t data);
    static void write(uint8_t *data, const uint32_t length);
    static void write(uint8_t data);

    static void waitWhileBusy();

    static void rotateCoordinates(uint32_t &x, uint32_t &y);
    static void mirrorCoordinates(uint32_t &x, uint32_t &y);
    static void rotateAndMirrorCoordinates(uint32_t &x, uint32_t &y);
    static bool isInBounds(const uint32_t x, const uint32_t y) { return ((x < display::widthInPixels) && (y < display::heightInPixels)); };
    static void swapCoordinates(uint32_t &c1, uint32_t &c2);
    static void mirrorCoordinate(uint32_t &c, const uint32_t maxC) { c = (maxC - 1) - c; };
    static uint32_t getByteOffset(const uint32_t x, const uint32_t y) { return ((y * widthInBytes) + (x / 8)); };
    static uint32_t getBitOffset(const uint32_t x) { return (7 - (x % 8)); };

    static void copyNewToOldFrameBuffer() {
        memcpy(oldFrameBuffer, newFrameBuffer, bufferSize);
    }

#ifdef generic
    static bool mockDisplayPresent;

#endif
};
