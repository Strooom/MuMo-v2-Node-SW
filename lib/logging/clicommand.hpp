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

    static constexpr uint32_t prompt{0};                                 // show build info and license
    static constexpr uint32_t help{'?'};                                 // show help
    static constexpr uint32_t gds{'g' * 65536 + 'd' * 256 + 's'};        // show config and status of the device
    static constexpr uint32_t gms{'g' * 65536 + 'm' * 256 + 's'};        // show recorded measurements status
    static constexpr uint32_t gls{'g' * 65536 + 'l' * 256 + 's'};        // show state of the LoRaWAN stack
    static constexpr uint32_t el{'e' * 256 + 'l'};                       // enable logging
    static constexpr uint32_t dl{'d' * 256 + 'l'};                       // disable logging
    static constexpr uint32_t er{'e' * 256 + 'r'};                       // enable radio
    static constexpr uint32_t dr{'d' * 256 + 'r'};                       // disable radio
    static constexpr uint32_t rml{'r' * 65536 + 'm' * 256 + 'l'};        // reset Mac Layer
    static constexpr uint32_t res{'r' * 65536 + 'e' * 256 + 's'};        // restart device - soft reset
    static constexpr uint32_t sda{'s' * 65536 + 'd' * 256 + 'a'};        // set device address  : sda 260BF180
    static constexpr uint32_t snk{'s' * 65536 + 'n' * 256 + 'k'};        // set network key     : snk 0DBC6EF938B83EB4F83C28E3CA7B4132
    static constexpr uint32_t sak{'s' * 65536 + 'a' * 256 + 'k'};        // set application key : sak 7E22AA54A7F4C0842861A13F1D161DE2
    static constexpr uint32_t sn{'s' * 256 + 'n'};                       // set name            : sn Mini001
    static constexpr uint32_t sb{'s' * 256 + 'b'};                       // set batteryType     : sb 0
    static constexpr uint32_t sr{'s' * 256 + 'r'};                       // set radioType       : sr 0
    static constexpr uint32_t sd{'s' * 256 + 'd'};                       // set display         : sd line# deviceIndex channelIndex
    static constexpr uint32_t ss{'s' * 256 + 's'};                       // set sensor          : sd deviceIndex channelIndex oversampling prescaler

    uint32_t nmbrOfArguments{0};
    char commandAsString[maxCommandLineLength];
    uint32_t commandHash{0};
    char arguments[maxNmbrOfArguments][maxArgumentLength];
};
