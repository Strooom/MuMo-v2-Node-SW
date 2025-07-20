// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once

#include <stdint.h>
#include <applicationevent.hpp>
#include <mainstate.hpp>

class mainControllerFiniteStateMachine {
  public:
    mainControllerFiniteStateMachine() = delete;
    static void run(mainState state, applicationEvent event);
    static void run(mainState state);

  private:
    mainState state;
    static constexpr void (*onExitState[nmbrOfMainStates])() = {
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr};

    static constexpr void (*onEntryState[nmbrOfMainStates])() = {
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr};

    static constexpr void (*onEventInState[nmbrOfMainStates][nmbrOfApplicationEvents])() = {
        {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},        // MainState::boot
        {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},        // MainState::networkCheck
        {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},        // MainState::idle
        {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},        // MainState::sampling
        {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},        // MainState::networking
        {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}         // MainState::fatalError
    };

    static constexpr mainState nextState[nmbrOfMainStates][nmbrOfApplicationEvents] = {
        {mainState::networkCheck, mainState::idle, mainState::sampling, mainState::networking, mainState::fatalError, mainState::idle},        // MainState::boot
        {mainState::boot, mainState::idle, mainState::sampling, mainState::networking, mainState::fatalError, mainState::idle},                // MainState::networkCheck
        {mainState::boot, mainState::networkCheck, mainState::sampling, mainState::networking, mainState::fatalError, mainState::idle},        // MainState::idle
        {mainState::boot, mainState::networkCheck, mainState::idle, mainState::networking, mainState::fatalError, mainState::idle},            // MainState::sampling
        {mainState::boot, mainState::networkCheck, mainState::idle, mainState::sampling, mainState::fatalError, mainState::idle},              // MainState::networking
        {mainState::boot, mainState::networkCheck, mainState::idle, mainState::sampling, mainState::networking, mainState::idle}               // MainState::fatalError
    };
};