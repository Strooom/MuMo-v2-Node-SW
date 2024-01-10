#include <screen.hpp>
#include <display.hpp>
#include <graphics.hpp>
#include <ux.hpp>
#include <font.hpp>

bool screen::isModified{false};
char screen::bigText[maxTextLength + 1][maxNumberOfLines]{};
char screen::smallText[maxTextLength + 1][maxNumberOfLines]{};

extern font roboto36bold;
extern font tahoma24bold;

void screen::show() {
    isModified = false;
    getContents();

    if (isModified) {
        drawContents();
    }
}

void screen::getContents() {
    for (uint32_t lineIndex = 0; lineIndex < maxNumberOfLines; lineIndex++) {
    }
}

void screen::drawContents() {
    display::initialize();

    graphics::drawFilledRectangle(ux::marginLeft, 49, display::widthInPixels - ux::marginLeft, 50, graphics::color::black);
    graphics::drawFilledRectangle(ux::marginLeft, 99, display::widthInPixels - ux::marginLeft, 100, graphics::color::black);
    graphics::drawFilledRectangle(ux::marginLeft, 149, display::widthInPixels - ux::marginLeft, 150, graphics::color::black);

    for (uint32_t lineIndex = 0; lineIndex < maxNumberOfLines; lineIndex++) {
        graphics::drawText(ux::marginLeft, 6 * (lineIndex * 50), roboto36bold, bigText[lineIndex]);
        graphics::drawText(100, 6 * (lineIndex * 50), roboto36bold, smallText[lineIndex]);
    }

    display::update(updateMode::full);
    display::goSleep();
}