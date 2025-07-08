// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include <stdarg.h>        // required so we can define functions with variable number of arguments

class logging {
  public:
    logging() = delete;
    static void initialize();
    enum class destination : uint32_t {        // We can send logging to the following destinations, when they are connected and enabled :
        none,
        swo,
        uart2,
        uart1
    };
    enum class source : uint32_t {        // We can send logging from the following sources, when they are enabled :
        applicationEvents = 0,
        sensorEvents      = 2,
        sensorData        = 3,
        displayEvents     = 4,
        displayData       = 5,
        eepromData        = 6,
        eepromEvents      = 7,
        lorawanEvents     = 16,
        lorawanData       = 17,
        lorawanMac        = 18,
        sx126xControl     = 24,
        sx126xBufferData  = 25,
        settings          = 29,
        error             = 30,
        criticalError     = 31
    };

    static constexpr uint32_t bufferLength{256};
    static void reset() {
        activeSources      = 0;
        activeDestinations = 0;
    }
    static uint32_t snprintf(const char *format, ...);
    static uint32_t snprintf(const source aSource, const char *format, ...);
    static uint32_t snprintf(const destination aDestination, const char *format, ...);

    static void enable(const source aSource) { activeSources = activeSources | (0x01 << static_cast<uint32_t>(aSource)); }
    static void disable(const source aSource) { activeSources = activeSources & ~(0x01 << static_cast<uint32_t>(aSource)); }
    static void enable(const destination aDestination) { activeDestinations = activeDestinations | (0x01 << static_cast<uint32_t>(aDestination)); }
    static void disable(const destination aDestination) { activeDestinations = activeDestinations & ~(0x01 << static_cast<uint32_t>(aDestination)); }
    static bool isActive(const source aSource) {
        uint32_t mask = 0x01 << static_cast<uint32_t>(aSource);
        return ((activeSources & mask) != 0);
    }
    static bool isActive(const destination aDestination) {
        uint32_t mask = 0x01 << static_cast<uint32_t>(aDestination);
        return ((activeDestinations & mask) != 0);
    }
    static bool isActive() {
        return (activeDestinations != 0);
    }
    static uint32_t getActiveSources() { return activeSources; }
    static void setActiveSources(const uint32_t aSources) { activeSources = aSources; }
    static uint32_t getActiveDestinations() { return activeDestinations; }
    static void setActiveDestinations(const uint32_t aDestinations) { activeDestinations = aDestinations; }

#ifndef unitTesting

  private:
#endif
    static char buffer[bufferLength];          // in this buffer the snprintf is expanded before the contents is being sent to one or more of the destinations
    static uint32_t activeSources;             // bitfield of active logging sources
    static uint32_t activeDestinations;        // bitfield of active logging destinations
    static void write(const uint32_t dataLength);
    static void write(const destination aDestination, const uint32_t dataLength);
};

const char *toString(logging::source aSource);
const char *toString(logging::destination aDestination);