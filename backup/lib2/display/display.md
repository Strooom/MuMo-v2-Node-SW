# Low-level display hardware vs 'High-level' graphics API

display.h / .cpp : 
* driving the displays electrical signals
* writing commands and raw data bytes to the display
* manipulating pixels on the display buffer
* flushing the display buffer towards the display hardware

graphics.h / .cpp
* translate higher level graphics commands into raw pixel commands : drawing objects
    * line
    * rectangle
    * text
    * image / icon


# Coordinate System
* 0,0 is left-bottom (OpenGL convention)
* x is running from left to right
* y is running from bottom to top


# Detecting Display
* drive RESET low
* detect if BUSY is HIGH (a pulldown is needed, so when no display present, this line stays low)

# Native orientation of the display is as follows
   ###############
   # 0,0 -> Y    #
#### |           #
#    v           # 
#    X           #
####             #
   #             #
   ###############

So a rotation of 270 (clockwise) is needed to bring X from left to right and Y from bottom to top

when writing a byte to the display : the MSbit is the *first* pixel

# pixel colors
* seems a '1' pixel is blank and a '0' pixel is black

# Fonts
* seems a font table covers all printable ASCII characters = [32, 126] (so degrees symbol 248 is not part of the standard font, but I could eg move it to 127)
* each character has a height (pixels), resulting in rows of bytes
* each character has a width (pixels), resulting in 1 or more bytes, eg Font 16x11 has 16 x 2 bytes per charachter. Each row has two bytes, of which only the first 11 pixels are used.

So in order to define such a font in C++ we have following class

class font {
    uint32_t pixelheight{16};
    uint32_t pixelwidth{11};
    uint32_t bytesPerRow{ceiling(width / 8)}
}

# Drawing text :
* draw a single charachter
* string is composed of characters at the right place
* with all possible transformations of the display, it's not really possible to directly write the bytes into the displayBuffer : you need to draw pixel by pixel
* when erasing (an area or the whole display), you only need to draw the active pixels
* otherwise you need to also draw the inactive pixels..
* drawing a character, is basically drawing a bitmap (of a certain size), so it makes sense to do draw bitmaps first and to use it for drawing character

Partial Update : instead of updating all pixels, you can only update the modified ones. In order to allow this, we need to keep track of lowerX, upperX, lowerY and upperY which define a rectangle constraining pixel modifications.