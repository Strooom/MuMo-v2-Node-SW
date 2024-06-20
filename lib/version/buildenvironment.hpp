#pragma once
#include <cstdint>

enum class buildEnvironment {
    unknown,
    local,
    ci,
};

const char* toString(buildEnvironment aBuildEnvironment);