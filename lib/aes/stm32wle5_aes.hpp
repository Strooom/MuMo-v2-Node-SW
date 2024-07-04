// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include <aeskey.hpp>
#include <aesblock.hpp>

enum class aesMode : uint32_t {
    EBC = 0,        // for encrypting a single block
    CBC = 1,        // for calculating the LoRaWAN MIC
    CTR = 2,        // for encrypting a LoRaWAN payload
};

class stm32wle5_aes {
  public:
    static void initialize(aesMode theMode = aesMode::EBC);
    static void enable();
    static void disable();
    static void setKey( aesKey& theKey);
    static void setInitializationVector( aesBlock& theBlock);
    static void write(aesBlock& theBlock);
    static void read(aesBlock& theBlock);
    static void write(uint32_t *wordsIn);
    static void read(uint32_t *wordsOut);
    static bool isComputationComplete();
    static void clearComputationComplete();
};