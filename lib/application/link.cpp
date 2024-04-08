// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include <link.hpp>

uint32_t linkQuality::rtcTicksSinceLastUpdate{rtcTicksBetweenUpdate + 1};

void linkQuality::tick() {
    rtcTicksSinceLastUpdate++;
}

bool linkQuality::needsUpdate() {
    return (rtcTicksSinceLastUpdate >= rtcTicksBetweenUpdate);
}
