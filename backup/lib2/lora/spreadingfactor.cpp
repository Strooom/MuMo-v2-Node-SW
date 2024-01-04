#include "spreadingfactor.h"

const char* toString(spreadingFactor aSpreadingFactor) {
    switch (aSpreadingFactor) {
        case spreadingFactor::SF7:
            return "SF7";
        case spreadingFactor::SF8:
            return "SF8";
        case spreadingFactor::SF9:
            return "SF9";
        case spreadingFactor::SF10:
            return "SF10";
        case spreadingFactor::SF11:
            return "SF11";
        case spreadingFactor::SF12:
            return "SF12";
        default:
            return "Unknown spreading factor";
    }
}