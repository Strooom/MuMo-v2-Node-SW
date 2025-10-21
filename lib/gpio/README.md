# GPIO Library

Hardware abstraction layer for GPIO management on the STM32WLE5 microcontroller in the MuMo environmental monitoring device.

## Overview

The GPIO library provides a simplified interface for managing GPIO pin groups used throughout the MuMo firmware. Rather than controlling individual pins, the library organizes related pins into logical groups (e.g., I2C, SPI, UART) and provides methods to enable/disable entire groups together. This approach:

- Simplifies power management by enabling/disabling peripherals as groups
- Ensures consistent pin configuration across the application
- Reduces code duplication when configuring related pins
- Supports hardware version differences (v2 vs v3)

## Architecture

The `gpio` class is a static-only utility class (constructor is deleted) that wraps STM32 HAL GPIO functions. All GPIO configuration is done through predefined groups defined in the `gpio::group` enum.

### Key Design Patterns

- **Static-only class**: No instances needed, all methods are static
- **Group-based control**: Pins are managed in functional groups rather than individually
- **Platform abstraction**: Uses conditional compilation (`#ifndef generic`) to support both native testing and embedded target
- **Hardware version support**: Conditional compilation (`#ifdef v3`) handles v2/v3 hardware differences

## API Reference

### Initialization

```cpp
static void initialize();
```

Enables the GPIO peripheral clocks for ports A, B, and C. Must be called during system initialization before using any GPIO functionality.

**Usage:**
```cpp
gpio::initialize();
```

### GPIO Groups

```cpp
enum class group : uint32_t {
    none = 0,
    rfControl,
    i2c,
    writeProtect,
    spiDisplay,
    debugPort,
    uart1,
    uart2,
    usbPresent,
    enableDisplayPower,
    enableSensorsEepromPower,
    test0
};
```

#### Group Descriptions

| Group | Pins | Function | Notes |
|-------|------|----------|-------|
| `rfControl` | PA4, PA5 | RF antenna control signals | Push-pull outputs for antenna switching |
| `i2c` | PA15 (SDA), PB15 (SCL) | I2C2 bus for sensors | Open-drain with alternate function |
| `writeProtect` | PB9 | EEPROM write protect | Active low, set HIGH to protect |
| `spiDisplay` | PA10 (MOSI), PB13 (SCK), PB14 (D/C), PB5 (CS), PA0 (Reset), PB10 (Busy) | SPI2 for e-paper display | Multiple pins for SPI and display control |
| `debugPort` | PA13 (SWDIO), PA14 (SWCLK), PB3 (SWO) | SWD debug interface | Disabled to save power when not debugging |
| `uart1` | PB6 (TX), PB7 (RX) | USART1 serial port | Used for test output on target hardware |
| `uart2` | PA2 (TX), PA3 (RX) | USART2 serial port | USB virtual COM port for CLI |
| `usbPresent` | PB4 | USB power detection | Interrupt-driven input with rising/falling edge detection |
| `enableDisplayPower` | PA9, PC0 | Display power control | v3 hardware only, soft-start for display rail |
| `enableSensorsEepromPower` | PC1 | Sensor/EEPROM power control | FET switch control |
| `test0` | PB0 | Test/debug output | General purpose test pin for oscilloscope/logic analyzer |

### Enable GPIO Group

```cpp
static void enableGpio(group aGroup);
```

Configures and enables all pins in the specified group. Sets appropriate modes (input/output/alternate function), pull-up/down resistors, speed, and initial states.

**Parameters:**
- `aGroup`: The GPIO group to enable

**Usage:**
```cpp
gpio::enableGpio(gpio::group::i2c);           // Enable I2C pins
gpio::enableGpio(gpio::group::spiDisplay);    // Enable display SPI
```

**Implementation details:**
- Configures pin modes (input, output, alternate function, interrupt)
- Sets initial pin states (typically LOW for outputs)
- Configures pull resistors where needed
- Sets up alternate functions for UART, SPI, I2C
- Handles interrupt configuration for `usbPresent` group
- v3 hardware performs soft-start sequence for display power

### Disable GPIO Group

```cpp
static void disableGpio(group aGroup);
```

De-initializes all pins in the specified group, returning them to their reset state (typically analog input) to minimize power consumption.

**Parameters:**
- `aGroup`: The GPIO group to disable

**Usage:**
```cpp
gpio::disableGpio(gpio::group::spiDisplay);   // Disable display when not in use
gpio::disableGpio(gpio::group::debugPort);    // Disable debug pins to save power
```

**Power management considerations:**
- Debug port pins (SWDIO, SWCLK, SWO) are disabled after boot to reduce power consumption
- Display pins are handled differently on v2 vs v3 hardware:
  - **v3**: All display pins can be disabled as power is removed via FET
  - **v2**: Reset pin must remain HIGH to prevent display wake-up from deep sleep
- `displayBusy` input is always disabled to save ~100µA

### Debug Probe Detection

```cpp
static bool isDebugProbePresent();
```

Checks if a debug probe (ST-Link, J-Link, etc.) is connected via the SWD interface.

**Returns:**
- `true` if debugger is connected and active
- `false` if no debugger is present or on native platform

**Usage:**
```cpp
if (!gpio::isDebugProbePresent()) {
    // Enter low-power mode - safe because no debugger will be disconnected
    enterStopMode();
}
```

**Implementation:**
Uses the CoreDebug register `DHCSR` (Debug Halting Control and Status Register) to check if the `C_DEBUGEN` bit is set.

## Pin Mapping Reference

### Port A Pins
| Pin | Function | Group | Direction | Notes |
|-----|----------|-------|-----------|-------|
| PA0 | Display Reset | spiDisplay | Output | |
| PA2 | UART2 TX | uart2 | AF7 | USB virtual COM |
| PA3 | UART2 RX | uart2 | AF7 | USB virtual COM |
| PA4 | RF Control 1 | rfControl | Output | |
| PA5 | RF Control 2 | rfControl | Output | |
| PA9 | Display power soft-start | enableDisplayPower | Output | v3 only |
| PA10 | SPI2 MOSI | spiDisplay | AF5 | |
| PA13 | SWDIO | debugPort | AF | Debug interface |
| PA14 | SWCLK | debugPort | AF | Debug interface |
| PA15 | I2C2 SDA | i2c | AF4 OD | |

### Port B Pins
| Pin | Function | Group | Direction | Notes |
|-----|----------|-------|-----------|-------|
| PB0 | Test pin | test0 | Output | Debug/measurement |
| PB3 | SWO | debugPort | AF | Serial wire output |
| PB4 | USB power detect | usbPresent | Input IT | Interrupt on both edges |
| PB5 | Display CS | spiDisplay | Output | Chip select |
| PB6 | UART1 TX | uart1 | AF7 | Test port |
| PB7 | UART1 RX | uart1 | AF7 | Test port |
| PB9 | Write protect | writeProtect | Output | EEPROM WP (active low) |
| PB10 | Display busy | spiDisplay | Input PD | Busy status from display |
| PB13 | SPI2 SCK | spiDisplay | AF5 | |
| PB14 | Display D/C | spiDisplay | Output | Data/command select |
| PB15 | I2C2 SCL | i2c | AF4 OD | |

### Port C Pins
| Pin | Function | Group | Direction | Notes |
|-----|----------|-------|-----------|-------|
| PC0 | Display power enable | enableDisplayPower | Output | v3 only, FET control |
| PC1 | Sensor/EEPROM power | enableSensorsEepromPower | Output | FET control |

## Usage Examples

### Basic Initialization

```cpp
// Early in main.cpp startup
gpio::initialize();

// Enable peripherals needed for operation
gpio::enableGpio(gpio::group::enableSensorsEepromPower);
HAL_Delay(10);  // Wait for power rail to stabilize

gpio::enableGpio(gpio::group::i2c);
gpio::enableGpio(gpio::group::writeProtect);
```

### Power Management

```cpp
// Before entering low-power mode
gpio::disableGpio(gpio::group::debugPort);     // Save power if no debugger
gpio::disableGpio(gpio::group::spiDisplay);    // Display in deep sleep
gpio::disableGpio(gpio::group::i2c);           // Sensors powered down

// Enter STOP2 mode
HAL_PWR_EnterSTOP2Mode(PWR_STOPENTRY_WFI);

// After waking up
gpio::enableGpio(gpio::group::i2c);
gpio::enableGpio(gpio::group::enableSensorsEepromPower);
```

### Display Management

```cpp
// Enable display (v3 hardware performs soft-start automatically)
gpio::enableGpio(gpio::group::enableDisplayPower);
gpio::enableGpio(gpio::group::spiDisplay);

// Use display...
display.update();

// Disable display to save power
gpio::disableGpio(gpio::group::spiDisplay);
gpio::disableGpio(gpio::group::enableDisplayPower);
```

### EEPROM Write Protection

```cpp
// Enable write protect (write protect is active LOW)
gpio::enableGpio(gpio::group::writeProtect);
HAL_GPIO_WritePin(GPIOB, writeProtect_Pin, GPIO_PIN_SET);  // WP disabled (can write)

// Perform write operations...

// Re-enable protection
HAL_GPIO_WritePin(GPIOB, writeProtect_Pin, GPIO_PIN_RESET);  // WP enabled (read-only)
```

## Hardware Dependencies

### Pin Definitions

The library relies on pin definitions from the STM32 HAL configuration in [main.h](../../include/main.h):
- Pin numbers (e.g., `rfControl1_Pin`, `I2C2_SDA_Pin`)
- GPIO ports (e.g., `I2C2_SDA_GPIO_Port`)
- Alternate function mappings

### Hardware Versions

The code supports two hardware versions with conditional compilation:

**v2 Hardware:**
- No display power control (display always powered when board is powered)
- Display reset pin must remain HIGH when display is sleeping
- No soft-start circuitry

**v3 Hardware:**
- FET-controlled display power rail (PA9, PC0)
- Soft-start circuit to prevent inrush current brownout
- All display pins can be fully disabled when powered off

To compile for v3 hardware, add `-D v3` to build flags.

## Power Consumption Considerations

Careful GPIO management is critical for achieving low power consumption:

1. **Debug port**: Disabling SWD pins saves significant power when no debugger is attached
2. **Display busy pin**: Disabling this input saves ~100µA when display is sleeping
3. **Unused UART**: UART1 RX is not used and should be disabled when not testing
4. **Analog mode**: `disableGpio()` returns pins to analog input mode for minimal current draw
5. **Pull resistors**: Inputs use pull-down resistors to prevent floating states

## Interrupt Configuration

### USB Power Detection

The `usbPresent` group configures PB4 as an interrupt-driven input:

```cpp
gpio::enableGpio(gpio::group::usbPresent);
```

This sets up:
- Rising and falling edge detection on PB4
- EXTI4 interrupt with priority 6
- Pull-down resistor (low when USB not present)

**Interrupt handler implementation** is required in [stm32wlxx_it.cpp](../../src/stm32wlxx_it.cpp):

```cpp
void EXTI4_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(usbPowerPresent_Pin);
}
```

The HAL callback `HAL_GPIO_EXTI_Callback()` must be implemented to handle the event.

### Adding Interrupts to Other Pins

To add interrupt capability to a GPIO pin:

1. **Set the pin mode** to interrupt type in `enableGpio()`:
   ```cpp
   GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;  // or IT_RISING, IT_FALLING
   ```

2. **Configure and enable the NVIC interrupt**:
   ```cpp
   HAL_NVIC_SetPriority(EXTIx_IRQn, priority, subpriority);
   HAL_NVIC_EnableIRQ(EXTIx_IRQn);
   ```

3. **Declare the IRQ handler** in [stm32wlxx_it.h](../../include/stm32wlxx_it.h):
   ```cpp
   extern "C" void EXTIx_IRQHandler(void);
   ```
   The `extern "C"` linkage prevents C++ name mangling.

4. **Implement the IRQ handler** in [stm32wlxx_it.cpp](../../src/stm32wlxx_it.cpp):
   ```cpp
   void EXTIx_IRQHandler(void) {
       HAL_GPIO_EXTI_IRQHandler(pin);
   }
   ```

5. **Implement the HAL callback** to process the interrupt:
   ```cpp
   void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
       if (GPIO_Pin == yourPin_Pin) {
           // Handle interrupt
       }
   }
   ```

See [gpio.md](gpio.md) for quick reference on interrupt setup steps.

## Platform Abstraction

The library uses conditional compilation to support both embedded target and native testing:

```cpp
#ifndef generic
    // STM32-specific code using HAL functions
#else
    // Native platform stubs (e.g., for unit tests)
#endif
```

On native platform:
- `initialize()` does nothing
- `enableGpio()` and `disableGpio()` do nothing
- `isDebugProbePresent()` always returns `false`

This allows application code to compile and run unit tests on a development machine without requiring embedded hardware.

## Integration with Other Libraries

The GPIO library is used by several other components:

- **[I2C library](../i2c/)**: Calls `enableGpio(gpio::group::i2c)` before I2C operations
- **[SPI library](../spi/)**: Calls `enableGpio(gpio::group::spiDisplay)` for display communication
- **[UART library](../cube/)**: Enables `uart1` or `uart2` groups for serial communication
- **[Power management](../application/)**: Disables groups before entering low-power modes
- **[Display](../display/)**: Controls display power rail and SPI pins

## Testing

The GPIO library is tested on both platforms:

**Generic tests** (native platform):
- Verify methods compile and link correctly
- Test that calls don't crash on native platform
- Located in [test/generic/](../../test/generic/)

**Target tests** (hardware required):
- Verify correct pin initialization and state
- Measure power consumption with groups enabled/disabled
- Test interrupt functionality
- Located in [test/target_no_jigs/](../../test/target_no_jigs/)

## Related Documentation

- Hardware design: [MuMo-V2-Node-PCB](https://github.com/Strooom/MuMo-V2-Node-PCB)
- STM32WLE5 reference manual: [RM0453](https://www.st.com/resource/en/reference_manual/rm0453-stm32wlex-advanced-armbased-32bit-mcus-with-subghz-radio-solution-stmicroelectronics.pdf)
- STM32 HAL GPIO documentation
- Project Wiki: [GitHub Wiki](https://github.com/Strooom/MuMo-v2-Node-SW/wiki)

## License

Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International
https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode
