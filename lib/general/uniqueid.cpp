// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <uniqueid.hpp>
#include <hexascii.hpp>
#include <logging.hpp>
#include <inttypes.h>

uint64_t uniqueId::get() {
    return ((static_cast<uint64_t>(UID->W1) << 32) | static_cast<uint64_t>(UID->W0));
}

void uniqueId::dump() {

    char tmpKeyAsHexAscii[17];
    hexAscii::uint64ToHexString(tmpKeyAsHexAscii, get());
    logging::snprintf("Device UID:  %s\n", tmpKeyAsHexAscii);
}
