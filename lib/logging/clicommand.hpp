// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include <stdlib.h>        // atoi

class cliCommand {
  public:
    uint32_t argumentAsUint32(uint32_t argumentIndex) const {
        if (argumentIndex < nmbrOfArguments) {
            return static_cast<uint32_t>(atoi(arguments[argumentIndex]));
        }
        return 0;
    }

    static constexpr uint32_t maxCommandLineLength{128};
    static constexpr uint32_t maxCommandLength{4};
    static constexpr uint32_t maxArgumentLength{36};
    static constexpr uint32_t maxNmbrOfArguments{4};

    static constexpr uint32_t prompt{0};                                                   // show build info and license
    static constexpr uint32_t help{'?'};                                                   // show help
    static constexpr uint32_t getDeviceStatus{'g' * 65536 + 'd' * 256 + 's'};              // show config and status of the device
    static constexpr uint32_t getMeasurementsStatus{'g' * 65536 + 'm' * 256 + 's'};        // show recorded measurements status
    static constexpr uint32_t getMeasurements{'g' * 256 + 'm'};                            // show recorded measurements : gm <offset = 0>
    static constexpr uint32_t getLoRaWANStatus{'g' * 65536 + 'l' * 256 + 's'};             // show state of the LoRaWAN stack
    static constexpr uint32_t enableLogging{'e' * 256 + 'l'};                              // enable logging
    static constexpr uint32_t disableLogging{'d' * 256 + 'l'};                             // disable logging
    static constexpr uint32_t enableRadio{'e' * 256 + 'r'};                                // enable radio
    static constexpr uint32_t disableRadio{'d' * 256 + 'r'};                               // disable radio
    static constexpr uint32_t resetMacLayer{'r' * 65536 + 'm' * 256 + 'l'};                // reset Mac Layer
    static constexpr uint32_t setDeviceAddress{'s' * 65536 + 'd' * 256 + 'a'};             // set device address  : sda 260BF180
    static constexpr uint32_t setNetworkKey{'s' * 65536 + 'n' * 256 + 'k'};                // set network key     : snk 0DBC6EF938B83EB4F83C28E3CA7B4132
    static constexpr uint32_t setApplicationKey{'s' * 65536 + 'a' * 256 + 'k'};            // set application key : sak 7E22AA54A7F4C0842861A13F1D161DE2
    static constexpr uint32_t setName{'s' * 256 + 'n'};                                    // set name            : sn Mini001
    static constexpr uint32_t setBattery{'s' * 256 + 'b'};                                 // set batteryType     : sb 0
    static constexpr uint32_t setRadio{'s' * 256 + 'r'};                                   // set radioType       : sr 0
    static constexpr uint32_t setDisplay{'s' * 256 + 'd'};                                 // set display         : sd line# deviceIndex channelIndex
    static constexpr uint32_t setSensor{'s' * 256 + 's'};                                  // set sensor          : sd deviceIndex channelIndex oversampling prescaler
    static constexpr uint32_t softwareReset{'s' * 65536 + 'w' * 256 + 'r'};                // restart device - soft reset
    static constexpr uint32_t hardwareReset{'h' * 256 + 'w' * 256 + 'r'};                  // reset device - arm for hard reset, reset occurs after removing USB to prevent going into bootloader mode

    uint32_t nmbrOfArguments{0};
    char commandAsString[maxCommandLineLength];
    uint32_t commandHash{0};
    char arguments[maxNmbrOfArguments][maxArgumentLength];
};
