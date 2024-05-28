// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include <main.h>

typedef struct
{
    volatile uint32_t W0;
    volatile uint32_t W1;

} UID_typeDef;

class uniqueId {
  public:
    static void dump();
    static uint64_t get();
};

#define UID ((UID_typeDef *)UID64_BASE)

