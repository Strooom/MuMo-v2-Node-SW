# Low-level display hardware vs 'High-level' graphics API

display.hpp / .cpp : 
* driving the displays electrical signals
* writing commands and raw data bytes to the display controller SSD1681
* manipulating pixels on the display buffer
* flushing the display buffer towards the display hardware

graphics.hpp / .cpp
* translate higher level graphics commands into raw pixel commands : drawing objects
    * line
    * rectangle
    * text
    * image / icon


# Coordinate System of the Display Buffer
* 0,0 is left-bottom (OpenGL convention)
* x is running from left to right
* y is running from bottom to top


# Detecting Display
* drive RESET low
* detect if BUSY is HIGH (a pulldown is needed, so when no display present, this line stays low)

# Native orientation of the display
When sending raw bytes to the display RAM, they are shown on the hardware depending on a setting 'Data Entry Mode' (command 0x11).
This setting controls how data from the external 5K displayBuffer is mapped to the display internal buffer during the SPI transfer
When set to 0b00, the first databyte is shown as (L = Least Significant Bit, M = Most Significant Bit):

   ###############
   #             #
####             #
#    L           #
#    .           #
#### .           #
   # M           #
   ###############

When set to 0b01, the first databyte is shown as :

   ###############
   # M           #
#### .           #
#    .           #
#    L           #
####             #
   #             #
   ###############

When set to 0b10, the first databyte is shown as :

   ###############
   #             #
####             #
#              L #
#              . #
####           . #
   #           M #
   ###############

When set to 0b11 (power-on default), the first databyte is shown as :

   ###############
   #           M #
####           . #
#              . #
#              L #
####             #
   #             #
   ###############

Note : when setting a Data Entry Mode with 'decrement', you need to set xStart, xEnd, yStart, yEnd to match this :
* for increment, eg xStart=0, xEnd=24
* for decrement, eg xStart=24, xEnd=0

The 'AM' bit (2) in Data Entry Mode controls where the next byte is shown : 
* 0 : next byte is shown further in the X-direction (example mode 0b011) 

   ###############
   #           0 #
####           0 #
#              0 #
#              0 #
#              1 #
#              1 #
#              1 #
#              1 #
#                #
####             #
   #             #
   ###############

* 1 : next byte is shown further in the Y-direction (example mode 0b111)

   ###############
   #          10 #
####          10 #
#             10 #
#             10 #
#                #
#                #
#                #
#                #
#                #
####             #
   #             #
   ###############

* 25 bytes (200 pixels / 8 bits-pixels per byte) form a complete row / column
* the next byte is shown on the next row / column
* a '1' bit shows as white, a '0' bit shows as black


To map the OpenGL coordinate system onto the display hardware, the following settings can work :
1. Data Entry Mode 0b00 + rotate 90 degrees
2. Data Entry Mode 0b11 + rotate 270 degrees

# Fonts
* Bitmapped fonts can be generated from TrueType fonts using the TheDotFactory.exe, a settings file is available in OutputConfigs.xml
* As these fonts are proportional, each font needs 3 pieces of data
  - fontProperties : height, spacing, first char, last char
  - characterProperties : an array with for each character the offset and length into the pixelData
  - pixelData


# Drawing text :
* draw a single character
* string is composed of characters at the right place
* with all possible transformations of the display, it's not really possible to directly write the bytes into the displayBuffer : you need to draw pixel by pixel
* when erasing (an area or the whole display), you only need to draw the active pixels
* otherwise you need to also draw the inactive pixels..
* drawing a character, is basically drawing a bitmap (of a certain size), so it makes sense to do draw bitmaps first and to use it for drawing character



# Icons
* open website https://fonts.google.com/icons
* set to 48 pixels
* download icon as PNG
* open then image2cpp https://javl.github.io/image2cpp/
* select icon file and generate code

# Partial Update
SSD1681 has 2 update modes :
* Mode 1 : full update. The display will cycle through all white, all black, new image, etc. to activate all pigments particles and results in a clean, high contrast image, gets rid of any ghosting. This update mode takes longer, depending on temperature, about 2500 ms
* Mode 2 : partial update. The display needs the old (current) image in RAM2 and the new image in RAM1. It will apply an update depending on the needed pixel statechange. This typically takes less time, about 1000ms, but it requires you to write old and new displayBuffer to the display. Also this mode creates less contrast, may leave a bit of ghosting image. Mode two can also be applied to a selected area instead of the whole display. You need to set xStart, xEnd, yStart, yEnd to mark the window with changes. There is a small gain in update time when updating a region io. the whole display, typically like 800 ms for an update.

# Faster update
* The display update time depends on the temperature : when colder, the pixel pigments move slower. So a trick to make the display update faster, is to NOT use the internal temperature sensor, but rather write the temperature, and make the display believe temp is 100 degrees. The update is the really quick, with very limited ghosting. To get rid of any ghosting, you could do a 'normal' update, every 'fullRefreshCount' fast updates.
Note : this trick does not seem to be so reliable.. not working good on all displays and at different temperatures...