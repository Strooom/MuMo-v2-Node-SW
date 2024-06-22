// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <cstdint>

class cliCommand {
  public:
    static constexpr uint32_t maxShortNameLength{8};
    static constexpr uint32_t maxLongNameLength{20};
    static constexpr uint32_t maxArgumentLength{36};
    static constexpr uint32_t maxNmbrOfArguments{4};

    const char shortName[maxShortNameLength];
    const char longName[maxLongNameLength];
    const uint32_t nmbrOfArguments;
    void (*const handler)();
};

// enum class cliCommand2 : uint32_t {
//     help = (0x68 << 24) + (0x65 << 16) + (0x6C << 8) + 0x70,        // show Help
//     stop = (0x73 << 24) + (0x74 << 16) + (0x6F << 8) + 0x70,        // stop LoRa to prepare for firmware update

//     ver = (0x76 << 16) + (0x65 << 8) + (0x72),        // show Version
//     hid = (0x68 << 16) + (0x69 << 8) + (0x64),        // show Hardeware IDs
//     lac = (0x6C << 16) + (0x61 << 8) + (0x63),        // List Available Channels
//     bat = (0x62 << 16) + (0x61 << 8) + (0x74),        // show Battery Status
//     net = (0x6E << 16) + (0x65 << 8) + (0x74),        // show Network Status
//     mem = (0x6D << 16) + (0x65 << 8) + (0x6D),        // show Memory Status
//     res = (0x72 << 16) + (0x65 << 8) + (0x73),        // restart software

//     snk = (0x73 << 16) + (0x6E << 8) + (0x6B),        // set Network Key
//     sak = (0x73 << 16) + (0x61 << 8) + (0x6B),        // set Application Key
//     sda = (0x73 << 16) + (0x64 << 8) + (0x61),        // set Device Address
//     sam = (0x73 << 16) + (0x61 << 8) + (0x6D),        // set Administator Mode
//     cam = (0x63 << 16) + (0x61 << 8) + (0x6D),        // clear Administator Mode
//     rml = (0x72 << 16) + (0x6D << 8) + (0x6C),        // reset Mac Layer : frameCounters reset to 0, rx1Delay reset to 1s and only default channels enabled
//     // TODO : add commands for enable/disable logging sources.

// };
