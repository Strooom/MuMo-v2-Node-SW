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
#include <logging.hpp>
#include <sensordevicecollection.hpp>
#include <display.hpp>
#include <gpio.hpp>
#include <power.hpp>
#include <settingscollection.hpp>

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
    state = mainState::idle;
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

    logging::snprintf("MuMo v2 - %s - Boot\n", version::getIsVersionAsString());
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

    logging::enable(logging::source::criticalError);
    logging::enable(logging::source::error);
    logging::enable(logging::source::applicationEvents);

}

void mainController::handleEvents() {
    while (applicationEventBuffer.hasEvents()) {
        applicationEvent theEvent = applicationEventBuffer.pop();
        logging::snprintf(logging::source::applicationEvents, "Application Event [%u] : %s\n", static_cast<uint8_t>(theEvent), toString(theEvent));
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
                // sensorDeviceCollection::run();
                //  measurementCollection::run();
                //   3. check if we should do a transmission, ie. we have data to send

                // sensorCollection::runResult theResult = theSensors.run();
                // if (theResult == sensorCollection::runResult::newMeasurements) {
                //     // 2. Send them as payload in an UpLink message
                //     uint8_t tempData[256]{};
                //     uint32_t tempDataIndex{0};

                //     tempData[tempDataIndex] = static_cast<uint8_t>(payloadEncodingVersion::mumo_v2_0);        // first byte of payload identifies how the rest should be interpreted
                //     tempDataIndex++;

                //     for (uint32_t measurementIndex = 0; measurementIndex < theSensors.actualNumberOfMeasurements; measurementIndex++) {        // for each measurement, store the 9 bytes (type, timestamp, value) in the payload
                //         tempData[tempDataIndex] = static_cast<uint8_t>(theSensors.latestMeasurements[measurementIndex].type);

                //         tempData[tempDataIndex + 1] = theSensors.latestMeasurements[measurementIndex].timestamp.asBytes[0];
                //         tempData[tempDataIndex + 2] = theSensors.latestMeasurements[measurementIndex].timestamp.asBytes[1];
                //         tempData[tempDataIndex + 3] = theSensors.latestMeasurements[measurementIndex].timestamp.asBytes[2];
                //         tempData[tempDataIndex + 4] = theSensors.latestMeasurements[measurementIndex].timestamp.asBytes[3];

                //         tempData[tempDataIndex + 5] = theSensors.latestMeasurements[measurementIndex].value.asBytes[0];
                //         tempData[tempDataIndex + 6] = theSensors.latestMeasurements[measurementIndex].value.asBytes[1];
                //         tempData[tempDataIndex + 7] = theSensors.latestMeasurements[measurementIndex].value.asBytes[2];
                //         tempData[tempDataIndex + 8] = theSensors.latestMeasurements[measurementIndex].value.asBytes[3];

                //         tempDataIndex += 9;
                //     }
                //     LoRaWAN::checkNetwork();
                //     LoRaWAN::sendUplink(0x10, tempData, tempDataIndex);
                // }

                // 2. check if we have enough unsent data to send uplink
                //     uint32_t maxUplinkPayloadNow =                     LoRaWAN::getMaxApplicationPayloadLength();
                //     uint32_t measurementToBeTransmitted =                     theMeasurements.getNmbrToBeTransmitted();
                //     if (((measurementToBeTransmitted + 1) * measurement::length) > maxUplinkPayloadNow) {
                //         logging::snprintf(
                //             "[%u] measurement bytes to transmit, [%u] bytes payload capacity\n",
                //             (measurementToBeTransmitted + 1) * measurement::length,
                //             maxUplinkPayloadNow);
                //         if (LoRaWAN::isReadyToTransmit()) {
                //             logging::snprintf("LoRaWAN layer ready to transmit\n");
                //             byteBuffer thePayload;        //
                //             thePayload.setFromHexAscii(
                //                 "000102030405060708090A0B0C0D0E0F");        // TODO - TEST msg
                //             LoRaWAN::sendUplink(thePayload, 0x10);
                //         }
                //     } else {
                //         logging::snprintf("Not enough data to transmit\n");
                //     }
            } break;

            default:
                break;
        }
    }
}
