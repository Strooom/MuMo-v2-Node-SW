#include "batterytype.h"

batteryType constrainToValidValue(uint32_t aBatteryTypeAsUint32) {
    if (aBatteryTypeAsUint32 < nmbrBatteryTypes) {
        return static_cast<batteryType>(aBatteryTypeAsUint32);
    } else {
        return batteryType::liFePO4_700mAh;
    }
}
