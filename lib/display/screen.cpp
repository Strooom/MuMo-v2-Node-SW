#include <screen.hpp>
#include <display.hpp>
#include <graphics.hpp>
#include <ux.hpp>
#include <font.hpp>

sensorChannelType screen::lines[maxNumberOfLines]{sensorChannelType::none, sensorChannelType::none, sensorChannelType::none, sensorChannelType::none};

extern font roboto16;
extern font roboto38bold;

void screen::show() {
    display::initialize();

    graphics::drawFilledRectangle(ux::marginLeft, 49, display::widthInPixels - ux::marginLeft, 50, graphics::color::black);
    graphics::drawFilledRectangle(ux::marginLeft, 99, display::widthInPixels - ux::marginLeft, 100, graphics::color::black);
    graphics::drawFilledRectangle(ux::marginLeft, 149, display::widthInPixels - ux::marginLeft, 150, graphics::color::black);

    for (uint32_t lineIndex = 0; lineIndex < maxNumberOfLines; lineIndex++) {
        graphics::drawText(ux::marginLeft, 6 * (lineIndex * 50), roboto38bold, getLastMeasurement(lines[lineIndex]));
    }

    display::update(updateMode::full);
    display::goSleep();
}

const char* screen::getLastMeasurement(sensorChannelType) {
    return "Some Text";
}