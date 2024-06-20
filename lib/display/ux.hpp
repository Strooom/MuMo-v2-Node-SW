// ######################################################################################
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

    // Spacing for 36px font
    static constexpr uint32_t marginBottomLarge{5};

    // Spacing for 24px font
    static constexpr uint32_t marginBottomSmall{13};

    // Spacing for 12px font
    static constexpr uint32_t marginBottomConsole{2};


    static constexpr uint32_t mid{display::widthInPixels / 2};

    // dimensions for battery icon
    static constexpr uint32_t batteryIconWidth{16};
    static constexpr uint32_t batteryIconHeight{36};
    static constexpr uint32_t batteryIconThickness{2};
    static constexpr uint32_t batteryIconGaugeHeight{batteryIconHeight - 2 * batteryIconThickness};
    static constexpr uint32_t batteryIconTipHeight{5};
    static constexpr uint32_t batteryIconTipWidth{8};
    static constexpr uint32_t batteryIconTipLeft{(batteryIconWidth - batteryIconTipWidth) / 2};

    // dimensions for network signal icon
    static constexpr uint32_t netwerkSignalStrengthBarWidth{6};
    static constexpr uint32_t netwerkSignalStrengthNumberOfBars{4};
    static constexpr uint32_t netwerkSignalStrengthBaseHeight{10};
    static constexpr uint32_t netwerkSignalStrengthStepHeight{10};
    static constexpr uint32_t netwerkSignalStrengthWidth{(netwerkSignalStrengthBarWidth * netwerkSignalStrengthNumberOfBars)};
    static constexpr uint32_t netwerkSignalStrengthHeight{netwerkSignalStrengthBaseHeight + (netwerkSignalStrengthStepHeight * (netwerkSignalStrengthNumberOfBars - 1))};
};
