// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ### 
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ### 
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ### 
// ######################################################################################

#pragma once
#include <stdint.h>
#include "infochanneltype.h"

class infoChannel {
  public:
    infoChannelType getType();

  private:
    infoChannelType theType;
    uint32_t prescaler;
    uint32_t oversampling;
};