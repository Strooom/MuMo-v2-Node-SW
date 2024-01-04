#include "version.h"
#include "buildinfo.h"
#include <stdio.h>

uint8_t version::isVersionMajor{0};
uint8_t version::isVersionMinor{0};
uint8_t version::isVersionPatch{0};
buildType version::theBuildType{buildType::unknown};
buildEnvironment version::theBuildEnvironment{buildEnvironment::unknown};

char version::isVersionString[maxVersionStringLength]{};

void version::setIsVersion() {
    isVersionMajor      = buildInfo::mainVersionDigit;
    isVersionMinor      = buildInfo::minorVersionDigit;
    isVersionPatch      = buildInfo::patchVersionDigit;
    theBuildType        = buildInfo::theBuildType;
    theBuildEnvironment = buildInfo::theBuildEnvironment;

    snprintf(isVersionString, maxVersionStringLength, "v%d.%d.%d", isVersionMajor, isVersionMinor, isVersionPatch);
}



