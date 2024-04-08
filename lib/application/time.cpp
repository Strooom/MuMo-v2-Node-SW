// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include <time.hpp>

uint32_t applicationTime::rtcTicksSinceLastUpdate{rtcTicksBetweenUpdate + 1};

void applicationTime::tick() {
    rtcTicksSinceLastUpdate++;
}

bool applicationTime::needsUpdate() {
    return (rtcTicksSinceLastUpdate >= rtcTicksBetweenUpdate);
}
