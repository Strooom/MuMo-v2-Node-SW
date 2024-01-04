#pragma once
#include "stdint.h"
#include "buildtype.h"
#include "buildenvironment.h"

class version {
  public:
    static uint8_t getIsVersionMajor() { return isVersionMajor; }
    static uint8_t getIsVersionMinor() { return isVersionMinor; }
    static uint8_t getIsVersionPatch() { return isVersionPatch; }
    static buildType getBuildType() { return theBuildType; }
    static buildEnvironment getBuildEnvironment() { return theBuildEnvironment; }
    static void setIsVersion();
    static const char* getIsVersionAsString() { return isVersionString; }

#ifndef unitTesting
  private:
#endif

    static uint8_t isVersionMajor;
    static uint8_t isVersionMinor;
    static uint8_t isVersionPatch;

    static buildType theBuildType;
    static buildEnvironment theBuildEnvironment;

    static constexpr uint8_t maxVersionStringLength{16U};
    static char isVersionString[maxVersionStringLength];
};
