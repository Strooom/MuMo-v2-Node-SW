#include <batterytype.hpp>

const char* toString(batteryType type) {
    switch (type) {
        case batteryType::liFePO4_700mAh:
            return "LiFePO4 700mAh";
        case batteryType::liFePO4_1500mAh:
            return "LiFePO4 1500mAh";
        case batteryType::alkaline_1200mAh:
            return "Alkaline 1200mAh";
        case batteryType::saft_ls_14250:
            return "Saft LS14250";
        case batteryType::saft_ls_145000:
            return "Saft LS14500";
        default:
            return "Unknown battery type";
    }
}