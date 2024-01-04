// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once

#include <stdint.h>
#include <mainstate.hpp>

class mainController {
  public:
    mainController() = delete;
    static void initialize();
    static void handleEvents();

#ifndef unitTesting

  private:
#endif
    static void initializeLogging();
    static mainState state;
};
