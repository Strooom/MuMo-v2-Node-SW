# Non-Volatile Storage (NVS) Library

## Overview

The NVS library provides a hardware-abstraction layer for persistent data storage using external I2C EEPROM chips. It handles the complexity of multi-bank addressing, page-boundary crossing, write protection, and I2C power management to provide a simple byte-array interface for storing configuration and measurement data.

## Hardware Architecture

### I2C EEPROM Configuration

The library supports multiple 64KB EEPROM banks connected via I2C:

- **Base I2C Address**: `0x50` (7-bit addressing)
- **Bank Addressing**: Each additional 64KB bank increments the I2C address (0x50, 0x51, 0x52, etc.)
- **Maximum Banks**: 8 banks (512KB total)
- **Internal Addressing**: 16-bit addressing within each 64KB bank
- **Page Size**: 128 bytes (configurable, but fixed at 128 for compatibility)

**Supported EEPROM Types**:
- BR24G512 (64KB)
- M24M01E (128KB, appears as 2x 64KB banks)
- Other compatible I2C EEPROM chips following the same addressing scheme

### Hardware Write Protection

The library interfaces with a hardware write-protect pin:
- Write operations temporarily disable write protection
- After each write, protection is re-enabled
- Prevents accidental data corruption from power glitches or software bugs

### Memory Map

The EEPROM is partitioned into two logical areas:

| Partition | Start Address | Size | Purpose |
|-----------|---------------|------|---------|
| **Settings** | 0x0000 | 4KB | Configuration data (LoRaWAN keys, sensor config, etc.) |
| **Measurements** | 0x1000 (4096) | Remaining capacity | Circular buffer for timestamped measurements |

The settings area is accessed via the `settingsCollection` abstraction layer.
The measurements area is managed by `measurementGroupCollection` as a circular buffer.

## Key Features

### 1. Automatic Bank Detection

On first access, the library scans the I2C bus to detect how many EEPROM banks are present:

```cpp
uint32_t nmbr64KBanks = nonVolatileStorage::getNmbr64KBanks();
```

Banks must be contiguous (no gaps in I2C addresses). If bank 0x51 is missing, bank 0x52 won't be detected even if physically present.

### 2. Page-Aware Operations

EEPROM devices can only write within a single page boundary in one operation. The library automatically handles:
- **Single-page operations**: Direct read/write for data within one page
- **Multi-page operations**: Automatically splits read/write operations that cross page boundaries
- **Multi-bank operations**: Handles addresses spanning multiple 64KB banks

### 3. I2C Power Management Integration

The library integrates with the I2C subsystem's power management:
- Automatically wakes I2C before operations if asleep
- Restores previous I2C power state after operations
- Enables low-power operation when EEPROM is not actively being accessed

### 4. Write Cycle Management

After each write operation, the library:
1. Disables write protection via GPIO
2. Performs the I2C write transaction
3. Waits for the EEPROM's internal write cycle to complete (4ms)
4. Re-enables write protection

This ensures data integrity and prevents incomplete writes.

### 5. Generic Platform Support

The library includes a mock EEPROM implementation for testing on native platforms:
```cpp
#ifdef generic
    static uint8_t mockEepromMemory[maxNmbr64KBanks * 64 * 1024];
#endif
```

This allows full unit testing without physical hardware.

## API Reference

### Initialization and Detection

```cpp
// Get number of 64KB banks detected (lazy initialization on first call)
uint32_t getNmbr64KBanks();

// Get total EEPROM size in bytes
uint32_t totalSize();

// Get available space for measurements (total - settings area)
uint32_t getMeasurementsAreaSize();
```

### Low-Level Read/Write Operations

**Note**: These are private methods, typically accessed through `settingsCollection` or `measurementGroupCollection`:

```cpp
// Single byte operations (simple but inefficient)
uint8_t read(const uint32_t address);
void write(const uint32_t address, const uint8_t data);

// Multi-byte operations (page-aware, efficient)
void read(const uint32_t startAddress, uint8_t* destination, const uint32_t dataLength);
void write(const uint32_t startAddress, const uint8_t* source, const uint32_t dataLength);
```

### Utility Operations

```cpp
// Fill entire EEPROM with 0xFF (blank state)
void erase();

// Fill entire EEPROM with a specific value
void fill(uint8_t value);
```

### Constants

```cpp
static constexpr uint32_t pageSize = 128;              // EEPROM page size
static constexpr uint8_t blankEepromValue = 0xFF;      // Erased EEPROM byte value
static constexpr uint32_t settingStartAddress = 0;     // Settings partition start
static constexpr uint32_t settingsSize = 4 * 1024;     // Settings partition size (4KB)
static constexpr uint32_t measurementsStartAddress = 4096; // Measurements start
```

## Usage Patterns

### Through settingsCollection (Recommended)

The NVS library is typically accessed indirectly through `settingsCollection`:

```cpp
// Save a configuration value
settingsCollection::save(someValue, settingsCollection::settingIndex::dataRate);

// Read a configuration value
uint8_t dataRate = settingsCollection::read<uint8_t>(settingsCollection::settingIndex::dataRate);

// Save a byte array (e.g., LoRaWAN keys)
uint8_t keyBytes[16];
settingsCollection::saveByteArray(keyBytes, settingsCollection::settingIndex::networkSessionKey);
```

### Through measurementGroupCollection

Measurements are stored in a circular buffer managed by `measurementGroupCollection`:

```cpp
// Initialize measurement storage
measurementGroupCollection::initialize();

// Add new measurement group
measurementGroupCollection::addNew(aMeasurementGroup);

// Read measurement group at specific offset
measurementGroup tmpGroup;
measurementGroupCollection::get(tmpGroup, offset);

// Get storage status
uint32_t freeSpace = measurementGroupCollection::getFreeSpace();
uint32_t oldestOffset = measurementGroupCollection::getOldestMeasurementOffset();
uint32_t newestOffset = measurementGroupCollection::getNewMeasurementsOffset();
```

## Implementation Details

### Address Calculation

The library uses helper functions to translate linear addresses into bank number, bank offset, and I2C address:

```cpp
uint16_t bankNumber(uint32_t address)       // Which 64KB bank (0-7)
uint16_t bankOffset(uint32_t address)       // Offset within bank (0-65535)
uint16_t bankI2cAddress(uint32_t address)   // I2C address (0x50 + bank number)
uint32_t pageNumber(uint32_t address)       // Which page (address / 128)
```

### Page Boundary Handling

When a read/write operation spans multiple pages, the library calculates how many bytes fit in the current page:

```cpp
uint32_t bytesInCurrentPage(uint32_t address, uint32_t dataLength) {
    uint32_t startPage = pageNumber(address);
    uint32_t endPage = pageNumber(address + dataLength);

    if (endPage == startPage) {
        return dataLength;  // All fits in one page
    } else {
        return pageSize - (address % pageSize);  // Bytes until end of current page
    }
}
```

The operation is then split into multiple page-aligned transactions.

### I2C Transaction Pattern

For physical hardware (non-generic):
1. Check if I2C is awake, wake if necessary
2. For writes: disable write protection
3. Perform HAL_I2C_Mem_Read or HAL_I2C_Mem_Write
4. For writes: wait for internal write cycle (HAL_Delay)
5. For writes: re-enable write protection
6. Restore previous I2C sleep state if needed

### Mock Implementation

In generic/native builds, the mock implementation uses a static array:
- Direct memory access via memcpy
- No I2C transactions
- No delays required
- Configurable bank count via `mockEepromNmbr64KPages`

## Error Handling

The library currently has minimal explicit error handling:
- Invalid addresses are passed directly to HAL (may cause HAL timeout)
- Failed I2C transactions return HAL error codes but aren't explicitly checked
- Detection stops at first missing bank (ensures contiguous addressing)

**Best practices**:
- Always check `getNmbr64KBanks() > 0` before using storage
- The main application goes to `fatalError` state if no EEPROM detected

## Power Consumption Considerations

The library is designed for ultra-low-power operation:
1. **I2C Power Management**: I2C bus is powered down when not in use
2. **Write Protection**: Hardware write protection prevents power-hungry write circuits from being active
3. **Batch Operations**: Multi-byte operations reduce I2C transaction overhead
4. **Selective Access**: Only wake I2C/EEPROM when actually needed

## Testing

The library includes extensive unit tests:
- Generic platform tests using mock EEPROM
- Target platform tests on actual hardware
- Tests cover: bank detection, page crossing, multi-bank operations, circular buffer wraparound

Test location: `test/generic/test_nvs/` and `test/target_no_jigs/test_nvs/`

## Related Components

- **settingsCollection** (`lib/settings/`): High-level settings management built on NVS
- **measurementGroupCollection** (`lib/sensors/`): Circular buffer for measurements using NVS
- **i2c** (`lib/i2c/`): I2C power management and low-level transactions
- **nonVolatileStorage** constants used by both higher-level abstractions

## Future Enhancements

Potential improvements noted in existing code:
- Use additional EEPROM banks beyond the first 64KB
- Store measurements in STM32WLE5 internal flash (256KB available)
- More explicit error handling and recovery
- Dynamic page size detection based on EEPROM type
- Wear leveling for frequently-written settings
