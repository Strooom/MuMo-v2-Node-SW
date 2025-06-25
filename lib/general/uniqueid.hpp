// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

typedef struct
{
    volatile uint32_t W0;
    volatile uint32_t W1;

} UID_typeDef;

class uniqueId {
  public:
    static uint64_t asUint64();
    static const char* asHexString();
#ifdef generic
    static uint64_t mockUniqueId;
#endif

  private:
    static char uidAsHexAscii[17];
};
