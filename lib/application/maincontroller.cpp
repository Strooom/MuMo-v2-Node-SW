// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <maincontroller.hpp>
#include <applicationevent.hpp>
#include <circularbuffer.hpp>
#include <version.hpp>
#include <buildinfo.hpp>
#include <logging.hpp>
#include <sensordevicecollection.hpp>
#include <display.hpp>
#include <gpio.hpp>
#include <power.hpp>
#include <settingscollection.hpp>
#include <screen.hpp>

#ifndef generic
#include "main.h"
#endif

mainState mainController::state{mainState::boot};
extern circularBuffer<applicationEvent, 16U> applicationEventBuffer;

void mainController::initialize() {
    version::setIsVersion();
    initializeLogging();

    // gpio::enableGpio(gpio::group::i2c);
    // gpio::enableGpio(gpio::group::writeProtect);
    // gpio::enableGpio(gpio::group::rfControl);

    sensorDeviceCollection::discover();

    if (nonVolatileStorage::isPresent()) {
        if (!settingsCollection::isInitialized()) {
            settingsCollection::initializeOnce();
        }
    }

    // gpio::enableGpio(gpio::group::spiDisplay);
    // if (display::isPresent()) {
    //     logging::snprintf("Display present\n");
    // } else {
    //     logging::snprintf("Display not present\n");
    // }

    // LoRaWAN::initialize(); // initialize the LoRaWAN network driver
    goTo(mainState::idle);
}

void mainController::initializeLogging() {
#ifndef generic
    if ((CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) == 0x0001) {        // if there is a SWD debugprobe connected...
        logging::enable(logging::destination::debugProbe);                     // enable the output to SWO
    }
#else
    logging::disable(logging::destination::debugProbe);

#endif

    gpio::enableGpio(gpio::group::usbPresent);
    if (power::hasUsbPower()) {                             // if there is USB power...
        logging::enable(logging::destination::uart);        // enable the output to UART
    }

    logging::snprintf("MuMo v2 - %s\n", version::getIsVersionAsString());
    logging::snprintf("%s %s build - %s\n", toString(version::getBuildEnvironment()), toString(version::getBuildType()), buildInfo::buildTimeStamp);
    logging::snprintf("Creative Commons 4.0 - BY-NC-SA\n");

    if (logging::isActive(logging::destination::debugProbe)) {
#ifndef generic
        LL_DBGMCU_EnableDBGStopMode();
#endif
        logging::snprintf("debugProbe connected\n");
    } else {
#ifndef generic
        LL_DBGMCU_DisableDBGStopMode();        // no debugging in low power -> the MCU will really stop the clock
#endif
        gpio::disableGpio(gpio::group::debugPort);        // these IOs are enabled by default after reset, but as there is no debug probe, we disable them to reduce power consumption
    }

    if (logging::isActive(logging::destination::uart)) {
        logging::snprintf("USB connected\n");
    }

    //logging::activeSources = settingsCollection::read<uint32_t>(settingsCollection::settingIndex::activeloggingSources);
}

void mainController::handleEvents() {
    while (applicationEventBuffer.hasEvents()) {
        applicationEvent theEvent = applicationEventBuffer.pop();
        logging::snprintf(logging::source::applicationEvents, "Application event : %s[%u]\n", toString(theEvent), static_cast<uint8_t>(theEvent));
        switch (theEvent) {
            case applicationEvent::usbConnected:
                // MX_USART2_UART_Init();
                break;

            case applicationEvent::usbRemoved:
                // MX_USART2_UART_DeInit();
                break;

            case applicationEvent::downlinkApplicationPayloadReceived: {
                // byteBuffer receivedData;
                // LoRaWAN::getDownlinkMessage(receivedData);
            } break;

            case applicationEvent::realTimeClockTick: {
                sensorDeviceCollection::tick();
            } break;

            default:
                break;
        }
    }
}

void mainController::run() {
    switch (state) {
        case mainState::measuring:
            sensorDeviceCollection::run();
            if (sensorDeviceCollection::isSleeping()) {
                goTo(mainState::logging);
            }
            break;

        case mainState::logging:
            if (sensorDeviceCollection::hasNewMeasurements()) {
                if (logging::isActive(logging::source::sensorData)) {
                    // log all new data
                }
            }
            goTo(mainState::storing);
            break;

        case mainState::storing:
            //measurementCollection::run();
            goTo(mainState::displaying);
            screen::show();
            break;

        case mainState::displaying:
            display::run();
            goTo(mainState::networking);
            break;

        case mainState::networking:
            //LoRaWAN::run();
            goTo(mainState::idle);
            break;

        case mainState::idle:
            // here we decide to go into deepSleep depending on external conditions
            // OR  goTo(mainState::sleeping);
            break;

        default:
            break;
    }
}

void mainController::goTo(mainState newState) {
    logging::snprintf(logging::source::applicationEvents, "MainController stateChange : %s[%d] -> %s[%d]\n", toString(state), static_cast<uint32_t>(state), toString(newState), static_cast<uint32_t>(newState));
    state = newState;
}
