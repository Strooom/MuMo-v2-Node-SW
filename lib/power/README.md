# Power Library

## Overview

The power library provides USB power detection and state management for the MuMo node firmware. It monitors the USB connection status through a GPIO pin and provides edge detection for USB connection and removal events. This functionality is critical for enabling USB-based features (CLI, firmware updates) and managing low-power sleep modes.

## Key Features

- USB power presence detection via GPIO
- Edge detection for USB connection and removal events
- Debouncing mechanism to prevent spurious wake-ups from sleep
- Mock support for unit testing on native platform

## Hardware Interface

On the target hardware (STM32WLE5), the library reads the `usbPowerPresent_Pin` on GPIOB to detect USB power. This signal is typically derived from VBUS detection circuitry on the USB connector.

## API Reference

### Public Methods

#### `bool hasUsbPower()`

Reads the current state of the USB power GPIO pin.

**Returns:**
- `true` if USB power is currently detected
- `false` if USB power is not present

**Behavior:**
- On target hardware: reads GPIOB `usbPowerPresent_Pin`
- On generic platform: returns `mockUsbPower` for testing
- Implements debouncing: sets counter to max when USB power is present, decrements when absent

**Usage:**
```cpp
if (power::hasUsbPower()) {
    // USB power is present - CLI available
    if (cli::hasCommand()) {
        // Process CLI commands
    }
}
```

#### `bool isUsbConnected()`

Detects the rising edge when USB power is connected.

**Returns:**
- `true` only once when USB transitions from disconnected to connected
- `false` otherwise (steady state or disconnected)

**Behavior:**
- Compares current USB power state to previous state
- Returns `true` on positive edge, then updates internal state
- Subsequent calls return `false` until next connection event

**Usage:**
```cpp
if (power::isUsbConnected()) {
    // USB just connected - trigger event
    applicationEventBuffer.push(applicationEvent::usbConnected);
    uart2::initialize();
    screen::setUsbStatus(true);
}
```

See: [maincontroller.cpp:580](../application/maincontroller.cpp#L580)

#### `bool isUsbRemoved()`

Detects the falling edge when USB power is removed.

**Returns:**
- `true` only once when USB transitions from connected to disconnected
- `false` otherwise (steady state or connected)

**Behavior:**
- Compares current USB power state to previous state
- Returns `true` on negative edge, then updates internal state
- Subsequent calls return `false` until next removal event

**Usage:**
```cpp
if (power::isUsbRemoved()) {
    // USB just removed - trigger event
    applicationEventBuffer.push(applicationEvent::usbRemoved);
    screen::setUsbStatus(false);
}
```

See: [maincontroller.cpp:585](../application/maincontroller.cpp#L585)

#### `bool noUsbPowerDelayed()`

Checks if USB power has been absent for a sustained period (debounced).

**Returns:**
- `true` if USB power has been off for at least `usbPowerOffCountMax` consecutive checks
- `false` if USB power is present or hasn't been off long enough

**Behavior:**
- Returns `true` only when `usbPowerOffCount` reaches 0
- Counter is reset to max (16) when USB power is detected
- Counter decrements on each call to `hasUsbPower()` when USB is absent

**Usage:**
```cpp
if (power::noUsbPowerDelayed() && !debugPort::isDebugProbePresent()) {
    prepareSleep();
    goSleep();  // Safe to enter low-power mode
}
```

See: [maincontroller.cpp:323](../application/maincontroller.cpp#L323)

**Purpose:** Prevents the device from immediately entering deep sleep when USB is briefly disconnected or during power glitches. This ensures stable operation and prevents spurious wake-ups.

## Implementation Details

### State Variables

- `usbPower` (static bool): Internal state tracking the last known USB power status
- `usbPowerOffCount` (static uint32_t): Debounce counter, initialized to `usbPowerOffCountMax` (16)
- `usbPowerOffCountMax` (static constexpr): Maximum count for debouncing (16 cycles)

### Debouncing Mechanism

The library implements a simple countdown-based debounce:

1. When USB power is detected, `usbPowerOffCount` is immediately set to 16
2. When USB power is absent, `usbPowerOffCount` decrements by 1 on each check
3. `noUsbPowerDelayed()` returns `true` only when the counter reaches 0

This mechanism requires 16 consecutive "USB absent" readings before allowing sleep mode, preventing false triggers from brief power interruptions.

### Platform Abstraction

The library uses conditional compilation to support both target hardware and native testing:

- **Target platform** (`#ifndef generic`): Reads actual GPIO pin via STM32 HAL
- **Generic platform** (`#ifdef generic`): Uses `mockUsbPower` boolean for testing

## Testing

Unit tests are located in [test/generic/test_power/test.cpp](../../test/generic/test_power/test.cpp).

**Run generic unit tests:**
```bash
pio test -e some_generic_unittests
```

**Test coverage:**
- Initial state verification
- USB connection edge detection
- USB removal edge detection
- State transitions and persistence

## Integration with Main Application

The power library is used by `mainController` in several key areas:

1. **USB event detection** ([maincontroller.cpp:579](../application/maincontroller.cpp#L579)): `runUsbPowerDetection()` polls for USB connection/removal and generates application events
2. **CLI availability** ([maincontroller.cpp:462](../application/maincontroller.cpp#L462)): Commands are processed only when USB power is present
3. **Sleep decision** ([maincontroller.cpp:323](../application/maincontroller.cpp#L323)): Device enters STOP2 low-power mode only when USB power is absent (with debounce) and no debugger is attached

## Power Management Context

The device has several power-related considerations documented in [power.md](power.md):

- Different peripherals (I2C, SPI, LPTIM1) are active during different operational phases
- USB connection affects sleep behavior and enables CLI
- Low-power modes are critical for battery operation

The power library is a key component in this power management strategy, enabling intelligent transitions between active and sleep states based on USB connectivity.

## Related Components

- [maincontroller.cpp](../application/maincontroller.cpp): Primary consumer of power state information
- [battery.cpp](../sensors/battery.cpp): Battery voltage monitoring (complementary power information)
- [gpio library](../gpio/): General GPIO abstraction layer
