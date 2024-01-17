// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ###
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#pragma once
#include <stdint.h>
#include <display.hpp>

class ux {
    public:
    // Divider
    static constexpr uint32_t marginLeft{10};
    static constexpr uint32_t dividerHeight{4};
    static constexpr uint32_t dividerLength{display::widthInPixels - (2 * marginLeft)};

// Spacing for 38px font
    static constexpr uint32_t marginBottomLarge{4};



// Spacing for 16px font
    static constexpr uint32_t marginBottomSmall{10};


};