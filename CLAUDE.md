# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is firmware for the MuMo node, a LoRaWAN-based environmental monitoring device for museum collection items, built on STM32WLE5 microcontroller. The project uses PlatformIO for building and includes both native tests (generic) and target-specific tests for the actual hardware.

Hardware repository: https://github.com/Strooom/MuMo-V2-Node-PCB
Wiki: https://github.com/Strooom/MuMo-v2-Node-SW/wiki

## Build Commands

### Local Development

Build the application for target hardware:
```bash
pio run -e target_mumo_v2x
```

Build and upload to target:
```bash
pio run -e target_mumo_v2x -t upload
```

### Testing

Run all generic unit tests (native platform, software AES):
```bash
pio test -e all_generic_unittests
```

Run specific generic unit tests:
```bash
pio test -e some_generic_unittests
```

Run target-specific unit tests (requires actual hardware and ST-Link):
```bash
pio test -e some_target_mumo_v2x_unittests
```

Run all target unit tests:
```bash
pio test -e all_target_mumo_v2x_unittests
```

### Other Commands

Generate compilation database (for code analysis tools):
```bash
pio run -t compiledb
```

Build production firmware (used in CI):
```bash
pio run -e production
```

## Test Configuration

Test port configuration is set in `platformio.ini`:
- `host_test_port`: Serial port for test output (e.g., COM5)
- `target_test_port`: Defines which UART to use on target (-D TARGET_TEST_PORT_UART1)
- Target tests require Unity framework and physical hardware with ST-Link debugger

To run a single generic test, edit `some_generic_unittests` environment in platformio.ini and uncomment the desired test filter.

## Code Architecture

### Core Application Structure

The application follows a state machine architecture coordinated by `mainController` (lib/application/maincontroller.cpp):

**Main states:**
- `boot`: Initial startup and configuration
- `idle`: Low-power waiting state
- `sampling`: Collecting measurements from sensors
- `networking`: Transmitting data via LoRaWAN
- `networkCheck`: Initial network validation on startup
- `fatalError`: Unrecoverable error state

**Event-driven design:**
- Events are managed through a circular buffer (`applicationEventBuffer`)
- Events include: RTC ticks, radio events (TX/RX complete, timeout), USB connection, downlink messages
- The main loop processes events, runs state machine logic, updates display, and enters low-power sleep

### LoRaWAN Implementation

The LoRaWAN stack (lib/lorawan/lorawan.cpp) implements Class A device functionality:

**Key components:**
- Handles ABP (Activation By Personalization) configuration
- Implements complete LoRaWAN 1.0.4 MAC layer including encryption/decryption (AES-CMAC)
- Manages frame counters, device addressing, and MIC validation
- Supports both hardware AES (STM32WLE5 crypto accelerator) and software AES
- Implements TX/RX cycle state machine with RX1 and RX2 receive windows
- MAC command processing (ADR, RX parameter setup, channel management, device time, etc.)
- Configuration and state persistence via non-volatile storage

**Dual AES implementation:**
- Hardware AES: Uses STM32WLE5 crypto peripheral (enabled via -D HARDWARE_AES)
- Software AES: Pure C++ implementation for testing on native platform
- Both implementations are tested to ensure compatibility

### Hardware Abstraction

The firmware uses a layered abstraction approach:

**STM32 HAL Layer (lib/cube/):**
- Thin C++ wrappers around STM32 HAL (I2C, SPI, UART, ADC, RTC, RNG, AES, LPTIM, SUBGHZ)
- These provide initialization and basic operations while maintaining HAL compatibility

**Application Layer:**
- Higher-level abstractions for peripherals (lib/gpio/, lib/i2c/, lib/spi/, lib/lptim/)
- Sensor device collection manages multiple I2C sensors (BME680, SHT40, SCD40, TSL2591, SPS30)
- Display abstraction (lib/display/) for optional SPI e-paper screen
- Power management with USB detection and low-power modes

### Sensor Management

The sensor subsystem (lib/sensors/) provides:
- Automatic I2C bus scanning and device discovery
- Per-channel configuration with oversampling and output frequency control
- Measurement aggregation into groups with timestamps
- Storage of measurements in non-volatile memory (EEPROM)
- Flexible mapping: display lines and LoRaWAN payloads can show any sensor channel

### Data Storage

Non-volatile storage (lib/nvs/) manages configuration and measurements:
- Settings stored in EEPROM (device address, keys, sensor config, display config, etc.)
- Circular buffer for measurement history
- Settings accessed via `settingsCollection` abstraction
- Measurement groups stored with timestamps and checksums

**Detailed documentation**: [lib/nvs/README.md](lib/nvs/README.md) - Covers I2C EEPROM architecture, page-aware operations, memory partitioning, and usage patterns.

### Radio Interface

SX126x radio driver (lib/sx126x/) interfaces with LoRa radio:
- Supports both SX1261 and SX1262 variants
- Uses STM32WLE5 SUBGHZ peripheral (internal radio)
- Handles modulation parameters, frequency, power, DIO interrupts
- Integrates with LoRaWAN stack for automatic channel selection and data rate adaptation

### Display System

Optional e-paper display support (lib/display/):
- Graphics rendering with custom font support
- Screen management with multiple screen types (logo, measurements, LoRa status, console)
- Bitmap and QR code rendering capabilities
- Updates only when content changes to save power

### Command Line Interface

USB-based CLI (lib/logging/cli.cpp) provides:
- Device status queries (sensors, LoRaWAN config, measurements)
- Configuration commands (device address, keys, sensor settings, display mapping)
- Logging control
- Measurement export (plain text and CSV formats)
- All commands parsed and executed via `mainController::runCli()`

## Build System Details

PlatformIO configuration (platformio.ini) defines multiple environments:

**Target environments:**
- `target_mumo_v2x`: Local development build with debugging symbols
- `production`: CI build for releases, optimized, hardware AES enabled
- Target tests: Hardware-in-loop testing with actual STM32WLE5 device

**Test environments:**
- `all_generic_unittests`: All native tests with code coverage
- `some_generic_unittests`: Selected native tests for rapid iteration
- Generic tests use software AES and run on desktop (native platform)

**Build flags:**
- `-D platformio`: Indicates PlatformIO build environment
- `-D unitTesting`: Enables test-specific code paths
- `-D debugBuild`: Debug build with symbols
- `-D HARDWARE_AES` / `-D SOFTWARE_AES`: Select AES implementation
- `-D generic`: Compiling for native platform (not embedded target)

## Version Management

Version is managed automatically in CI (see .github/workflows/testbuildrelease.yml):
- Semantic versioning (major.minor.patch)
- Commit message determines version increment: "major", "minor", or patch (default)
- Build info generated at compile time (lib/version/buildinfo.cpp)
- Build timestamp and commit hash embedded in firmware

**Detailed documentation**: [lib/version/README.md](lib/version/README.md) - Covers dual build workflows (local Python script vs CI), version determination logic, git tag format, and runtime usage patterns.

## Code Review Guidelines (from .github/copilot-instructions.md)

When reviewing or modifying code:
- Check for public functions that could be private
- Check for default constructors that could be deleted
- Check for use of magic numbers (should be named constants)
- Check for function parameters that could be const or const references

## Testing Strategy

**Generic Tests (Native Platform):**
- Pure C++ logic testing without hardware dependencies
- Uses software AES implementation
- Includes LoRaWAN protocol tests, sensor data processing, display rendering
- Code coverage reports generated (gcovr)

**Target Tests (Hardware Required):**
- Tests requiring actual STM32WLE5 hardware
- Two categories:
  - `target_no_jigs`: Tests that run without external test equipment
  - `target_with_jigs`: Tests requiring external test fixtures
- Uses Unity test framework with hardware-specific test port configuration

## Project Structure

- `lib/`: Component libraries organized by functionality (aes, lorawan, sensors, display, etc.)
- `src/`: Main application entry point (main.cpp) with HAL initialization
- `test/`: Unit tests organized by category (generic/, target_no_jigs/, target_with_jigs/, tools/)
- `include/`: STM32 HAL configuration headers
- `boards/`: Custom board definition for PlatformIO
- `.github/workflows/`: CI/CD configuration including test, build, and release automation

## Important Notes

- The firmware uses conditional compilation extensively (`#ifndef generic`) to support both native testing and embedded target
- LoRaWAN configuration uses ABP (not OTAA) - DevAddr and session keys must be provisioned via CLI
- Network check at boot validates LoRaWAN connectivity before entering normal operation
- Real-time clock synchronization via LoRaWAN DeviceTimeAnswer MAC command
- USB connection detection triggers bootloader entry for firmware updates
- The device enters STOP2 low-power mode when idle and no USB/debugger is connected
