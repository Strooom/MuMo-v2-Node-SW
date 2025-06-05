// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <uniqueid.hpp>
#include <hexascii.hpp>
#include <logging.hpp>
#include <inttypes.h>

    char uniqueId::uidAsHexAscii[17]{};

#ifndef generic
#include <main.h>
#define UID ((UID_typeDef*)UID64_BASE)
#else
uint64_t uniqueId::mockUniqueId{0x1122334455667788};
#endif

uint64_t uniqueId::asUint64() {
#ifndef generic
    return ((static_cast<uint64_t>(UID->W1) << 32) | static_cast<uint64_t>(UID->W0));
#else
    return mockUniqueId;
#endif
}

const char* uniqueId::asHexString() {
    hexAscii::uint64ToHexString(uidAsHexAscii, uniqueId::asUint64());
    return uidAsHexAscii;
}
