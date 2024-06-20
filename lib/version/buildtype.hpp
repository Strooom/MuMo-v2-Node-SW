#pragma once
#include <cstdint>

enum class buildType {
    unknown,
    development,
    production,
    integrationTest,
};

const char* toString(buildType aBuildType);