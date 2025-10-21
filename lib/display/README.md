# Display Library

This library provides a complete display subsystem for the MuMo node, featuring a 200x200 pixel e-paper display driven by the SSD1681 controller. The library is organized in three architectural layers: hardware driver, graphics API, and screen management.

## Architecture

### Three-Layer Design

```
┌─────────────────────────────────────────┐
│  Screen Layer (screen.cpp/hpp)          │  High-level UI screens
│  - Logo, measurements, console, etc.    │
├─────────────────────────────────────────┤
│  Graphics Layer (graphics.cpp/hpp)      │  Drawing primitives
│  - Lines, rectangles, text, bitmaps     │
├─────────────────────────────────────────┤
│  Display Driver (display.cpp/hpp)       │  Hardware control
│  - SSD1681 controller                   │
│  - Frame buffer management              │
│  - Pixel manipulation                   │
└─────────────────────────────────────────┘
```

### 1. Display Driver Layer ([display.hpp](display.hpp), [display.cpp](display.cpp))

The lowest level provides direct hardware control:

**Core Features:**
- SSD1681 e-paper controller communication via SPI
- Dual frame buffer management (200x200 pixels, 1 bit per pixel = 5KB per buffer)
- Pixel-level manipulation with rotation and mirroring support
- Full and partial display refresh modes
- Presence detection and power management

**Key Functions:**
```cpp
void detectPresence();              // Detect if display is connected
void setPixel(uint32_t x, uint32_t y);
void clearPixel(uint32_t x, uint32_t y);
void clearAllPixels();
void update(bool full);             // Update display (full or partial)
void goSleep();                     // Put display in low-power mode
```

**Display Specifications:**
- Resolution: 200x200 pixels
- Display type: Monochrome e-paper
- Controller: SSD1681
- Interface: SPI
- Power: Low power with sleep mode support

### 2. Graphics Layer ([graphics.hpp](graphics.hpp), [graphics.cpp](graphics.cpp))

Provides high-level drawing primitives:

**Drawing Primitives:**
```cpp
void drawPixel(uint32_t x, uint32_t y, color theColor);
void drawHorizontalLine(uint32_t xStart, uint32_t xEnd, uint32_t y, color theColor);
void drawVerticalLine(uint32_t x, uint32_t yStart, uint32_t yEnd, color theColor);
void drawLine(uint32_t xStart, uint32_t yStart, uint32_t xEnd, uint32_t yEnd, color theColor);
void drawRectangle(uint32_t xStart, uint32_t yStart, uint32_t xEnd, uint32_t yEnd, color theLineColor);
void drawFilledRectangle(uint32_t xStart, uint32_t yStart, uint32_t xEnd, uint32_t yEnd, color theFillColor);
void drawCircle(uint32_t x, uint32_t y, uint32_t radius, color theColor);
void drawFilledCircle(uint32_t x, uint32_t y, uint32_t radius, color theColor);
```

**Text and Images:**
```cpp
void drawCharacter(uint32_t xStart, uint32_t yStart, const font& theFont, uint8_t theCharacter);
void drawText(uint32_t xStart, uint32_t yStart, const font& theFont, const char* theText);
uint32_t getTextwidth(const font& theFont, const char* theText);
void drawBitMap(uint32_t xStart, uint32_t yStart, const bitmap& theBitmap);
```

**Icons and UI Elements:**
```cpp
void drawBatteryIcon(uint32_t xStart, uint32_t yStart, uint32_t level);            // level: 0-100
void drawNetworkSignalStrengthIcon(uint32_t xStart, uint32_t yStart, uint32_t level);  // level: 0-100
void drawQrCode(uint32_t xStart, uint32_t yStart);
```

### 3. Screen Layer ([screen.hpp](screen.hpp), [screen.cpp](screen.cpp))

Manages complete screen layouts and user interface:

**Screen Types** ([screentype.hpp](screentype.hpp)):
- `logo`: Startup logo screen
- `version`: Firmware version information
- `uid`: Device unique identifier (with QR code)
- `hwConfig`: Hardware configuration
- `measurements`: Sensor measurements display
- `loraConfig`: LoRaWAN configuration
- `loraStatus`: LoRaWAN status
- `main`: Main information screen

**Screen Management:**
```cpp
void setType(screenType newType);                  // Switch to different screen
void update();                                     // Render current screen
bool isModified();                                 // Check if screen needs update

// Measurement screen
void setText(uint32_t lineIndex, const char* textBig, const char* textSmall);
void setUsbStatus(bool hasUsbPower);
void setNetworkStatus(uint32_t networkSignalStrength);
void setBatteryStatus(uint32_t batteryLevel);

// Console screen
void setText(uint32_t lineIndex, const char* text);
void clearConsole();

// Device info
void setName(const char* newName);
void setUidAsHex(const char* newUidAsHex);
```

## Coordinate System

The display uses an OpenGL-style coordinate system:
- Origin (0,0) is at **bottom-left**
- X-axis runs from **left to right** (0 to 199)
- Y-axis runs from **bottom to top** (0 to 199)

```
  Y
  ^
  |
199 ┌────────────────┐
    │                │
    │                │
    │                │
  0 └────────────────┘ → X
    0              199
```

## Display Update Modes

The SSD1681 controller supports two refresh modes:

### Full Refresh (Mode 1)
- Complete display update cycle
- Cycles through all-white, all-black phases to eliminate ghosting
- High contrast, clean image
- Slower: approximately 2500ms (temperature dependent)
- Use for: Initial boot, periodic refresh to clear ghosting

### Partial Refresh (Mode 2)
- Fast update of changed pixels
- Requires both old and new frame buffers
- Faster: approximately 1000ms
- Lower contrast, may show slight ghosting
- Use for: Frequent updates, measurements, status changes

### Automatic Refresh Strategy
The driver implements automatic refresh management:
```cpp
void update();  // Automatically performs full refresh every 8 partial updates
```

This balances update speed with image quality by preventing ghosting buildup.

## Fonts

The library supports custom proportional bitmap fonts.

### Included Fonts
- **Roboto 36B** ([roboto36b.hpp](roboto36b.hpp)): Large bold font for primary data
- **Tahoma 24B** ([tahoma24b.hpp](tahoma24b.hpp)): Medium bold font for labels
- **Lucida Console 12** ([lucidaconsole12.hpp](lucidaconsole12.hpp)): Small monospace for console

### Font Structure

Each font consists of three components:

1. **Font Properties** ([font.hpp](font.hpp)):
   - Height in pixels
   - Spacing between characters
   - ASCII range (start/end codes)

2. **Character Properties**:
   - Width in pixels for each character
   - Offset into pixel data array

3. **Pixel Data**:
   - Bitmap data for all characters

### Generating Custom Fonts

Use **TheDotFactory.exe** with the provided configuration:
1. Open [OutputConfigs.xml](OutputConfigs.xml) in TheDotFactory
2. Select your TrueType font
3. Configure size and style
4. Generate C/C++ header file
5. Adapt to the font class structure

**Font resources:**
- Tool: TheDotFactory.exe
- Config: [OutputConfigs.xml](OutputConfigs.xml)

## Bitmaps and Icons

### Bitmap Class ([bitmap.hpp](bitmap.hpp), [bitmap.cpp](bitmap.cpp))

Represents monochrome bitmap images:
```cpp
bitmap(uint32_t width, uint32_t height, const uint8_t* pixelData);
bool getPixel(uint32_t x, uint32_t y) const;
```

### Included Icons
- **Logo**: [logo.hpp](logo.hpp) - MuMo logo (sources: [logo-large.jpg](logo-large.jpg), [logo-small.jpg](logo-small.jpg))
- **USB Icon**: [usbicon.hpp](usbicon.hpp) - USB connection indicator (source: [usbicon.jpg](usbicon.jpg))

### Creating Custom Icons

**Method 1: From Google Fonts Icons**
1. Visit [Google Fonts Icons](https://fonts.google.com/icons)
2. Set size to 48 pixels
3. Download icon as PNG

**Method 2: Using image2cpp**
1. Open [image2cpp](https://javl.github.io/image2cpp/)
2. Upload your PNG file
3. Select settings:
   - Code output format: Arduino/C++
   - Color format: 1 bit per pixel (monochrome)
   - Draw mode: Horizontal
4. Generate code
5. Adapt to bitmap class format

**Method 3: Using LVGL Image Converter**
1. Open [LVGL Image Converter](https://lvgl.io/tools/imageconverter)
2. Use version 8
3. Select color format: `CF_INDEXED_1_BIT`
4. Upload and convert your image

## User Experience (UX) Constants ([ux.hpp](ux.hpp))

Defines consistent spacing and layout constants:

```cpp
// Margins and spacing
static constexpr uint32_t marginLeft{10U};
static constexpr uint32_t marginBottomLarge{5U};    // For 36px font
static constexpr uint32_t marginBottomSmall{13U};   // For 24px font
static constexpr uint32_t marginBottomConsole{2U};  // For 12px font

// Layout helpers
static constexpr uint32_t mid{100U};                // Center point
static constexpr uint32_t mid60{120U};              // 60% point

// Icon dimensions
static constexpr uint32_t batteryIconWidth{16U};
static constexpr uint32_t batteryIconHeight{36U};
static constexpr uint32_t netwerkSignalStrengthWidth{16U};
static constexpr uint32_t netwerkSignalStrengthHeight{40U};

// QR code
static constexpr uint32_t qrCodeScale{6U};          // Pixel scaling factor

// Timing
static constexpr uint32_t timeToReaddisplay{3000U}; // ms user needs to read screen
```

## Display Hardware Details

### SSD1681 Controller

The SSD1681 is a single-chip driver IC for e-paper displays with:
- Embedded 160 x 296 bit RAM for graphics display data
- SPI interface
- Built-in oscillator and power management
- Support for partial update

**Key Commands** (from [display.hpp:61-85](display.hpp#L61-L85)):
- `DRIVER_OUTPUT_CONTROL` (0x01)
- `DATA_ENTRY_MODE_SETTING` (0x11)
- `WRITE_RAM_0` (0x24) - Write to display RAM
- `WRITE_RAM_1` (0x26) - Write to RAM for partial update
- `DISPLAY_UPDATE_CONTROL_1/2` (0x21/0x22)
- `MASTER_ACTIVATION` (0x20) - Trigger display update

### Data Entry Modes

The display controller supports 4 data entry modes (0x11 command) that control how SPI data maps to display pixels:

- **Mode 0b00**: LSB at bottom-left, data flows right
- **Mode 0b01**: LSB at top-left, data flows right
- **Mode 0b10**: LSB at bottom-right, data flows left
- **Mode 0b11**: LSB at top-right, data flows left (power-on default)

The library uses coordinate transformation to map the OpenGL coordinate system to the hardware's native orientation.

### Display Detection

Detection uses a simple but effective method:
1. Drive RESET pin low
2. Check BUSY pin state
3. If BUSY is high, display is present (internal pull-down confirms presence)

## Rotation and Mirroring

The display supports transformations for flexible mounting:

**Rotation** (clockwise):
- 0 degrees
- 90 degrees
- 180 degrees
- 270 degrees

**Mirroring**:
- None
- Horizontal
- Vertical
- Both

These transformations are applied automatically during coordinate translation.

## Frame Buffer Management

### Dual Buffer System

The driver maintains two frame buffers:
```cpp
static uint8_t newFrameBuffer[5000];  // Current frame being drawn
static uint8_t oldFrameBuffer[5000];  // Previous frame for partial updates
```

**Buffer size calculation:**
- 200 × 200 pixels = 40,000 pixels
- 1 bit per pixel = 40,000 bits = 5,000 bytes

### Update Flow

1. **Drawing Phase**: Application draws to `newFrameBuffer`
2. **Comparison**: For partial updates, compare with `oldFrameBuffer`
3. **Transfer**: Send buffer(s) to display via SPI
4. **Update**: Trigger display refresh
5. **Copy**: Move `newFrameBuffer` to `oldFrameBuffer` for next cycle

## Power Management

The display supports low-power operation:

```cpp
void goSleep();  // Put display into deep sleep mode
```

**Deep sleep mode:**
- Minimal power consumption
- Display retains image without power (e-paper characteristic)
- Wake via hardware reset or initialization sequence

## Usage Examples

### Basic Display Initialization

```cpp
#include <display.hpp>
#include <graphics.hpp>
#include <screen.hpp>

// Detect and initialize display
display::detectPresence();

if (display::isPresent()) {
    // Display is connected and ready
}
```

### Drawing Graphics

```cpp
#include <graphics.hpp>
#include <tahoma24b.hpp>

// Clear screen
display::clearAllPixels();

// Draw some shapes
graphics::drawFilledRectangle(10, 10, 100, 50, graphics::color::black);
graphics::drawCircle(150, 150, 30, graphics::color::black);

// Draw text
graphics::drawText(20, 100, tahoma24b, "Hello MuMo!");

// Update display
display::update();
```

### Using Screen Layer

```cpp
#include <screen.hpp>

// Show logo at startup
screen::setType(screenType::logo);
screen::update();

// Switch to measurements
screen::setType(screenType::measurements);
screen::setText(0, "23.5", "°C");
screen::setText(1, "45.2", "%RH");
screen::setText(2, "1013", "hPa");
screen::setBatteryStatus(75);  // 75%
screen::setNetworkStatus(80);  // Good signal
screen::setUsbStatus(false);
screen::update();
```

### Measurement Display with Status Icons

```cpp
// Update sensor readings
screen::setText(0, "23.5", "°C");
screen::setText(1, "45.2", "%RH");
screen::setText(2, "1013", "hPa");

// Update status
screen::setBatteryStatus(batteryLevel);          // 0-100
screen::setNetworkStatus(signalStrength);        // 0-100
screen::setUsbStatus(usbConnected);

// Render if changed
if (screen::isModified()) {
    screen::update();
}
```

### Console Output

```cpp
// Show console screen
screen::setType(screenType::main);
screen::clearConsole();

// Add lines of text
screen::setText(0, "Initializing...");
screen::setText(1, "Sensors: OK");
screen::setText(2, "Radio: OK");
screen::setText(3, "Ready!");

screen::update();
```

## Testing Support

The display library fully supports unit testing on native (non-embedded) platforms:

**Conditional Compilation:**
```cpp
#ifdef generic
    // Mock implementations for native testing
    static bool mockDisplayPresent;
#endif
```

**Test Access:**
```cpp
#ifndef unitTesting
  private:
#endif
    // Implementation details exposed for testing
```

This allows comprehensive testing of display logic without actual hardware.

## Performance Considerations

### Update Speed
- **Full refresh**: ~2500ms (temperature dependent)
- **Partial refresh**: ~1000ms
- **Partial region**: ~800ms (limited benefit)

### Optimization Tips
1. **Batch updates**: Make all changes before calling `update()`
2. **Use partial refresh**: For frequent updates (measurements, status)
3. **Smart refresh scheduling**: Only update when `screen::isModified()`
4. **Auto-refresh strategy**: Let driver handle full vs. partial (every 8th update)
5. **Sleep when inactive**: Use `goSleep()` to conserve power

### Memory Usage
- Frame buffers: 10KB (2 × 5KB)
- Display driver: Minimal stack usage
- No dynamic allocation

## Hardware Integration

The display interfaces with the STM32WLE5 microcontroller via:

**SPI Interface:**
- Clock, MOSI, MISO lines
- Chip Select (CS)
- Data/Command select (DC)

**Control Signals:**
- RESET: Hardware reset
- BUSY: Status indicator (display is updating)

**Power:**
- 3.3V supply
- Low current consumption
- Sleep mode support

## Related Documentation

- [CLAUDE.md](../../CLAUDE.md) - Project overview and build instructions
- [display.md](display.md) - Low-level technical notes about display hardware
- SSD1681 Datasheet - Display controller specifications

## Troubleshooting

### Display Not Detected
- Check BUSY pin has pull-down resistor
- Verify SPI connections
- Ensure proper power supply (3.3V)

### Ghosting Issues
- Use full refresh periodically (automatic every 8 updates)
- Avoid very frequent partial updates
- Consider environmental temperature effects

### Slow Updates
- Normal behavior for e-paper technology
- Full refresh is inherently slower
- Partial updates are faster but with quality trade-off
- Temperature affects refresh speed

### Display Not Updating
- Check if `display::isReady()` returns true
- Verify SPI communication
- Ensure display is not in sleep mode
- Check BUSY signal properly monitored

## License

CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/

## Author

Pascal Roobrouck - https://github.com/Strooom
