// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <uniqueid.hpp>
#include <hexascii.hpp>
#include <logging.hpp>
#include <inttypes.h>

#ifndef generic
#include <main.h>
#define UID ((UID_typeDef *)UID64_BASE)
#else
uint64_t mockUniqueId{0x1234};
#endif

uint64_t uniqueId::get() {
#ifndef generic
    return ((static_cast<uint64_t>(UID->W1) << 32) | static_cast<uint64_t>(UID->W0));
#else
    return mockUniqueId;
#endif
}

void uniqueId::dump() {
    char tmpKeyAsHexAscii[17];
    hexAscii::uint64ToHexString(tmpKeyAsHexAscii, get());
    logging::snprintf("Device UID:  %s\n", tmpKeyAsHexAscii);
}
