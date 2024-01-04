#pragma once
#include <stdint.h>

enum class updateMode : uint8_t {
    full    = 0xF7,
    fast    = 0xC7,
    partial = 0xFF
};