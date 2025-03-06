// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################
// based on the work of Richard Moore : https://github.com/ricmoo/QRCode
// based on the work of Nayuki : https://www.nayuki.io/page/qr-code-generator-library

#pragma once
#include <stdint.h>

enum class encodingFormat {
    numeric,
    alphanumeric,
    byte,
    nmbrOfEncodingFormats
};

static constexpr uint32_t nmbrOfEncodingFormats{static_cast<uint32_t>(encodingFormat::nmbrOfEncodingFormats)};