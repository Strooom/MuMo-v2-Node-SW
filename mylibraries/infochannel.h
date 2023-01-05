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