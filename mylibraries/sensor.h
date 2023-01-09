// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ### 
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ### 
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ### 
// ######################################################################################

#pragma once
#include <stdint.h>

// Base class for a sensor - All sensor classes derive from it to get their common methods

class sensor {
  public:
    bool initialize();
	bool isPresent();
};