# Low-level display hardware vs 'High-level' graphics API

display.hpp / .cpp : 
* driving the displays electrical signals
* writing commands and raw data bytes to the display
* manipulating pixels on the display buffer
* flushing the display buffer towards the display hardware

graphics.hpp / .cpp
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
* Bitmapped fonts can be generated from TrueType fonts using the TheDotFactory.exe, a settings file is available in OutputConfigs.xml
* As these fonts are proportional, each font needs 3 pieces of data
  - fontProperties : height, spacing, first char, last char
  - characterProperties : an array with for each character the offset and lenght into the pixelData
  - pixelData


# Drawing text :
* draw a single charachter
* string is composed of characters at the right place
* with all possible transformations of the display, it's not really possible to directly write the bytes into the displayBuffer : you need to draw pixel by pixel
* when erasing (an area or the whole display), you only need to draw the active pixels
* otherwise you need to also draw the inactive pixels..
* drawing a character, is basically drawing a bitmap (of a certain size), so it makes sense to do draw bitmaps first and to use it for drawing character

# Partial Update : instead of updating all pixels, you can only update the modified ones. In order to allow this, we need to keep track of lowerX, upperX, lowerY and upperY which define a rectangle constraining pixel modifications.


# Icons
* open website https://fonts.google.com/icons
* set to 48 pixels
* download icon as PNG
* open then image2cpp https://javl.github.io/image2cpp/
* select icon file and generate code