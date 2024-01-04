#include <screen.hpp>
#include <display.hpp>
#include <graphics.hpp>
#include <ux.hpp>
#include <font.hpp>

extern font roboto16;
extern font roboto38bold;

void screen::show() {
    display::initialize();

    graphics::drawFilledRectangle(ux::marginLeft, 49, display::widthInPixels - ux::marginLeft, 50, graphics::color::black);
    graphics::drawFilledRectangle(ux::marginLeft, 99, display::widthInPixels - ux::marginLeft, 100, graphics::color::black);
    graphics::drawFilledRectangle(ux::marginLeft, 149, display::widthInPixels - ux::marginLeft, 150, graphics::color::black);
    graphics::drawText(ux::marginLeft, 156, roboto38bold, "20");
    char tmp[8] = "5 C";
    tmp[1]      = 127;
    graphics::drawText(ux::marginLeft + 60, 156 + 15, roboto16, tmp);

    graphics::drawText(ux::marginLeft, 106, roboto38bold, "72");
    graphics::drawText(ux::marginLeft + 60, 106 + 15, roboto16, "%RH");

    graphics::drawText(ux::marginLeft, 56, roboto38bold, "50");
    graphics::drawText(ux::marginLeft + 60, 56 + 15, roboto16, "LUX");

    graphics::drawText(ux::marginLeft, 0, roboto16, "TEXTLINE1");
    graphics::drawText(ux::marginLeft, 0 + 23, roboto16, "TEXTLINE2");

    display::update(updateMode::full);
    display::goSleep();
}

