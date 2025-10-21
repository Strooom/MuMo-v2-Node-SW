# Logging and CLI Library

This library provides diagnostic logging and command-line interface capabilities for the MuMo firmware, enabling both development-time debugging and runtime device configuration via USB.

## Overview

The logging and CLI subsystem consists of several components:

- **Logging**: Flexible debug output system with configurable sources and destinations
- **CLI**: Command parser for interactive device configuration and status queries
- **UART1**: Transmit-only logging interface (e.g., for diagnostic output)
- **UART2**: Full duplex interface for CLI commands and responses
- **Debug Port**: SWO (Serial Wire Output) support for development/debugging

## Architecture

### Logging System

The logging system allows selective enabling/disabling of log messages from various subsystems and routing them to different output destinations.

**Key features:**
- Selective enable/disable by source and destination
- Multiple simultaneous output destinations
- Formatted output using `snprintf` syntax
- Minimal overhead when logging is disabled

**Log Sources:**
```cpp
enum class source : uint32_t {
    applicationEvents,   // Main state machine events
    sensorEvents,        // Sensor lifecycle events
    sensorData,          // Raw sensor measurements
    displayEvents,       // Display operations
    displayData,         // Display content
    eepromData,          // EEPROM read/write operations
    eepromEvents,        // EEPROM lifecycle events
    lorawanEvents,       // LoRaWAN state machine
    lorawanData,         // LoRaWAN packet contents
    lorawanMac,          // MAC command processing
    sx126xControl,       // Radio control operations
    sx126xBufferData,    // Radio buffer contents
    settings,            // Configuration changes
    error,               // Non-critical errors
    criticalError        // Critical errors (always enabled)
};
```

**Log Destinations:**
```cpp
enum class destination : uint32_t {
    none,    // No output
    swo,     // Serial Wire Output (debug probe only)
    uart2,   // USB-CDC interface (CLI port)
    uart1    // Diagnostic UART
};
```

### CLI System

The CLI provides a text-based interface for configuring and monitoring the device over USB. Commands are received via UART2, parsed, and executed by the main controller.

**Command Flow:**
1. User types command and presses Enter
2. UART2 receives characters into RX circular buffer
3. Newline (`\n`) triggers command counter increment
4. Main controller detects command via `cli::hasCommand()`
5. Command is retrieved and parsed into `cliCommand` structure
6. Main controller executes command via `mainController::runCli()`
7. Response is sent back via `cli::sendResponse()`

## Usage Examples

### Logging

**Basic initialization:**
```cpp
// Initialize with default settings (error logging to UART1)
logging::initialize();
```

**Enable specific log sources:**
```cpp
// Enable LoRaWAN event logging
logging::enable(logging::source::lorawanEvents);
logging::enable(logging::destination::uart2);

// Log a message
logging::snprintf(logging::source::lorawanEvents, "Joining network...\n");
```

**Enable multiple destinations:**
```cpp
// Send logs to both SWO (debugger) and UART2 (USB)
logging::enable(logging::destination::swo);
logging::enable(logging::destination::uart2);
```

**Unconditional logging:**
```cpp
// Log regardless of source filtering (uses destination filtering only)
logging::snprintf("System initialized\n");
```

**Runtime control:**
```cpp
// Disable all logging temporarily
logging::reset();

// Re-enable specific subsystem
logging::enable(logging::source::error);
logging::enable(logging::destination::uart1);
```

### CLI Commands

**Available Commands:**

| Command | Arguments | Description |
|---------|-----------|-------------|
| `<enter>` | - | Show build info and license |
| `?` | - | Show help |
| `gds` | - | Get device status (sensors, battery, radio) |
| `gms` | - | Get measurements status (count, timestamps) |
| `gm` | `[csv]` | Get measurements (optionally as CSV) |
| `gls` | - | Get LoRaWAN status |
| `el` | - | Enable logging to USB |
| `dl` | - | Disable logging |
| `er` | - | Enable radio |
| `dr` | - | Disable radio |
| `rml` | - | Reset MAC layer |
| `sda` | `<address>` | Set device address (hex) |
| `snk` | `<key>` | Set network key (32 hex chars) |
| `sak` | `<key>` | Set application key (32 hex chars) |
| `sn` | `<name>` | Set device name |
| `sb` | `<type>` | Set battery type |
| `sr` | `<type>` | Set radio type |
| `sd` | `<line> <dev> <ch>` | Set display line mapping |
| `ss` | `<dev> <ch> <filt> <freq>` | Set sensor configuration |
| `swr` | - | Software reset |

**Example Session:**
```
<enter>

https://github.com/Strooom
v2.5.1 - #fa4a6e6
target_mumo_v2x debug build - 2025-10-21 14:32:10
Creative Commons 4.0 - BY-NC-SA
Type '?' for help

gds
UID     : 003A00214D501220363038FF
name    : Mini001
display : present
EEPROM  : 2 * 64K present
battery : LiSOCl2_3600mAh (0)
radio   : SX1262 (1)
RTC     : Mon Oct 21 14:30:15 2025
[0] BME680
  [0] Temperature [°C] : averaging 4 samples, output every 15 minutes
  [1] Humidity [%RH] : averaging 4 samples, output every 15 minutes
  [2] Pressure [hPa] : averaging 4 samples, output every 15 minutes

sda 260BF180
device address set : 260BF180

el
logging enabled
```

### Command Implementation

**Adding a new CLI command:**

1. Define command hash in [clicommand.hpp](clicommand.hpp):
```cpp
static constexpr uint32_t myCommand{'m' * 256 + 'c'};  // "mc"
```

2. Implement handler in `mainController::runCli()`:
```cpp
case cliCommand::myCommand:
    handleMyCommand(theCommand);
    break;
```

3. Add help text in `mainController::showHelp()`:
```cpp
cli::sendResponse("mc <arg> : my command description\n");
```

## Implementation Details

### Command Parsing

Commands are parsed using a space-separated format:
- First segment: command (max 4 characters)
- Subsequent segments: arguments (max 4 arguments, 36 chars each)
- Commands are hashed for efficient comparison
- Case-insensitive parsing

**Command Hash Calculation:**
```cpp
// "gds" becomes: 'g' * 65536 + 'd' * 256 + 's' = 0x676473
uint32_t hash = cli::hash("gds");
```

### UART Interfaces

**UART2 (CLI Port):**
- Full-duplex operation
- RX buffer: 128 bytes (command buffer)
- TX buffer: 2048 bytes (response buffer)
- Interrupt-driven transmission and reception
- Command terminator: `\n` (newline)
- Connected to USB-CDC interface

**UART1 (Logging Port):**
- Transmit-only
- TX buffer: 2048 bytes
- Interrupt-driven transmission
- Used for diagnostic output

**Circular Buffers:**
Both UARTs use interrupt-driven circular buffers for efficient data handling:
- `rxNotEmpty()`: Called on receive interrupt, pushes data into RX buffer
- `txEmpty()`: Called on transmit interrupt, pops data from TX buffer
- `startOrContinueTx()`: Initiates/resumes transmission

### Debug Port (SWO)

Serial Wire Output provides low-overhead logging during development:
- Uses ARM CoreSight ITM (Instrumentation Trace Macrocell)
- No impact on UART pins
- Requires debug probe connection
- Automatically detected via `debugPort::isDebugProbePresent()`
- **Important**: After flashing via SWD, power-cycle (not reset) required for lowest power consumption

## Configuration and Settings

**Default Configuration:**
```cpp
void logging::initialize() {
    enable(logging::destination::uart1);
    enable(logging::source::error);
    enable(logging::source::criticalError);
}
```

**Runtime CLI Control:**
- `el`: Enables `logging::destination::uart2` (logs to USB)
- `dl`: Disables `logging::destination::uart2`

**Persistent Settings:**
Logging configuration is not persisted to EEPROM. It resets to defaults on each boot but can be changed at runtime via CLI.

## Buffer Management

**Buffer Sizes:**
- Logging snprintf buffer: 256 bytes
- CLI response buffer: 256 bytes
- UART1 TX buffer: 2048 bytes
- UART2 TX buffer: 2048 bytes
- UART2 RX buffer: 128 bytes

**Overflow Handling:**
- CLI response buffer: Blocks until space available (polling with 25ms delay)
- UART buffers: Circular buffers prevent overflow by limiting accepted data

## Testing

**Unit Tests:**
- `test/generic/test_cli/`: CLI parsing and command handling
- `test/generic/test_logging/`: Logging enable/disable and output

**Test Coverage:**
- Command line parsing (arguments, separators, hashing)
- UART TX/RX with mock interrupts
- Source/destination enable/disable
- Buffer management

**Running Tests:**
```bash
# Run all generic tests including logging/CLI
pio test -e all_generic_unittests

# Run specific test
pio test -e some_generic_unittests -f test_cli
```

## Platform Support

**Embedded Target (STM32WLE5):**
- Full hardware UART support
- SWO via ARM CoreSight
- Interrupt-driven operation

**Native Platform (Testing):**
- Mock UART with simulated interrupts
- Mock character transmission/reception
- No SWO support (returns immediately)

**Conditional Compilation:**
- `#ifndef generic`: Target-specific hardware access
- `#ifdef generic`: Mock implementations for testing

## Performance Considerations

- **Zero overhead when disabled**: If no destinations are enabled, `logging::snprintf()` returns immediately
- **Efficient hashing**: Command lookup uses compile-time constant hashing (O(1))
- **Interrupt-driven I/O**: UARTs don't block main execution
- **Formatted output**: Uses standard `vsnprintf` for flexibility

## Integration Points

**Main Controller:**
The CLI integrates with the main controller state machine in [maincontroller.cpp](../application/maincontroller.cpp):

```cpp
// In main loop (idle state)
if (power::hasUsbPower() && cli::hasCommand()) {
    cliCommand theCommand;
    cli::getCommand(theCommand);
    runCli(theCommand);
}
```

**Logging Usage Throughout Codebase:**
Various subsystems use logging for diagnostics:
- LoRaWAN stack: Network events, packet data, MAC commands
- Sensor collection: Device discovery, measurements
- Display: Rendering operations
- EEPROM: Read/write operations
- Radio: Control and buffer contents

## Troubleshooting

**No CLI response:**
- Check USB connection (`power::hasUsbPower()`)
- Verify UART2 initialization
- Check baud rate configuration
- Ensure command terminates with `\n`

**Logging not appearing:**
- Verify destination is enabled: `logging::isActive(destination)`
- Verify source is enabled: `logging::isActive(source)`
- Check physical connection (USB for UART2, debug probe for SWO)
- For SWO: Ensure debugger is attached

**Commands not recognized:**
- Commands are case-insensitive (automatically converted to lowercase)
- Verify command hash matches constant in `clicommand.hpp`
- Check maximum command length (4 characters)
- Ensure space separation between command and arguments

## References

- Command definitions: [clicommand.hpp](clicommand.hpp)
- CLI implementation: [cli.cpp](cli.cpp), [cli.hpp](cli.hpp)
- Logging implementation: [logging.cpp](logging.cpp), [logging.hpp](logging.hpp)
- UART drivers: [uart1.hpp](uart1.hpp), [uart2.hpp](uart2.hpp)
- Debug port: [debugport.hpp](debugport.hpp)
- Main controller integration: [lib/application/maincontroller.cpp](../application/maincontroller.cpp)
- Unit tests: [test/generic/test_cli/](../../test/generic/test_cli/), [test/generic/test_logging/](../../test/generic/test_logging/)
