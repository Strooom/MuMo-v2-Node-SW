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
    static constexpr uint32_t marginLeft{10U};
    static constexpr uint32_t dividerHeight{4U};
    static constexpr uint32_t dividerLength{display::widthInPixels - (2U * marginLeft)};

    // Spacing for 36px font
    static constexpr uint32_t marginBottomLarge{5U};

    // Spacing for 24px font
    static constexpr uint32_t marginBottomSmall{13U};

    // Spacing for 12px font
    static constexpr uint32_t marginBottomConsole{2U};

    static constexpr uint32_t mid{display::widthInPixels / 2U};
    static constexpr uint32_t mid60{(display::widthInPixels * 60U) / 100U};

    // icon width / spacing
    static constexpr uint32_t iconWidth{16U};
    static constexpr uint32_t iconSpacing{20U};

    // dimensions for battery icon
    static constexpr uint32_t batteryIconWidth{16U};
    static constexpr uint32_t batteryIconHeight{36U};
    static constexpr uint32_t batteryIconThickness{2U};
    static constexpr uint32_t batteryIconGaugeHeight{batteryIconHeight - 2U * batteryIconThickness};
    static constexpr uint32_t batteryIconTipHeight{5U};
    static constexpr uint32_t batteryIconTipWidth{8U};
    static constexpr uint32_t batteryIconTipLeft{(batteryIconWidth - batteryIconTipWidth) / 2U};

    // dimensions for network signal icon
    static constexpr uint32_t netwerkSignalStrengthBarWidth{4U};
    static constexpr uint32_t netwerkSignalStrengthNumberOfBars{4U};
    static constexpr uint32_t netwerkSignalStrengthBaseHeight{10U};
    static constexpr uint32_t netwerkSignalStrengthStepHeight{10U};
    static constexpr uint32_t netwerkSignalStrengthWidth{(netwerkSignalStrengthBarWidth * netwerkSignalStrengthNumberOfBars)};
    static constexpr uint32_t netwerkSignalStrengthHeight{netwerkSignalStrengthBaseHeight + (netwerkSignalStrengthStepHeight * (netwerkSignalStrengthNumberOfBars - 1U))};

    // qrCode
    static constexpr uint32_t qrCodeScale{6U};
    static constexpr uint32_t qrCodeXOffset = (display::widthInPixels - (qrCodeScale * 25U)) / 2U;
    static constexpr uint32_t qrCodeYOffset = 30U;

    // Logo
    static constexpr uint32_t logoMargin{4U};

    // Timing
    static constexpr uint32_t timeToReaddisplay{3000U};
};
