# Float Utility Library

A collection of utility functions for working with floating-point numbers in embedded systems, particularly useful for formatting, serialization, and conversion operations.

## Overview

This library provides functions to:
- Extract integer and fractional parts from floats with controlled precision
- Convert between float values and byte arrays (IEEE 754 format)
- Generate scaling factors for decimal precision control

These utilities are particularly useful in embedded systems where:
- Display formatting requires specific decimal precision
- Data must be serialized for transmission (e.g., LoRaWAN payloads)
- Memory-efficient storage of measurement data is needed

## Functions

### Integer and Fractional Part Extraction

#### `integerPart()`
```cpp
int32_t integerPart(const float value, const uint32_t decimals)
```

Extracts the integer part of a float value with rounding based on specified decimal precision.

**Parameters:**
- `value`: The float value to process
- `decimals`: Number of decimal places to consider for rounding (0-N)

**Returns:** Integer part of the value, rounded according to the specified decimal precision

**Examples:**
```cpp
integerPart(10.4F, 0)   // Returns 10 (rounds down)
integerPart(10.5F, 0)   // Returns 11 (rounds up)
integerPart(10.95F, 1)  // Returns 11 (9.5 in first decimal rounds up)
integerPart(-10.5F, 0)  // Returns -11 (rounds away from zero)
```

#### `fractionalPart()`
```cpp
uint32_t fractionalPart(const float value, const uint32_t decimals)
```

Extracts the fractional part of a float value with specified decimal precision.

**Parameters:**
- `value`: The float value to process
- `decimals`: Number of decimal places to extract (0-N)

**Returns:** Fractional part as an unsigned integer (always positive, even for negative inputs)

**Examples:**
```cpp
fractionalPart(10.44F, 1)   // Returns 4
fractionalPart(10.45F, 2)   // Returns 45
fractionalPart(-10.44F, 1)  // Returns 4 (absolute value)
fractionalPart(10.49F, 0)   // Returns 0 (no decimals requested)
```

**Note:** The fractional part is always positive, regardless of the sign of the input value.

### Scaling Factor Helpers

#### `factorFloat()`
```cpp
float factorFloat(const uint32_t decimals)
```

Generates a floating-point scaling factor for a given number of decimal places.

**Parameters:**
- `decimals`: Number of decimal places

**Returns:** 10^decimals as a float

**Examples:**
```cpp
factorFloat(0)  // Returns 1.0
factorFloat(1)  // Returns 10.0
factorFloat(2)  // Returns 100.0
```

#### `factorInt()`
```cpp
uint32_t factorInt(const uint32_t decimals)
```

Generates an integer scaling factor for a given number of decimal places.

**Parameters:**
- `decimals`: Number of decimal places

**Returns:** 10^decimals as an unsigned integer

**Examples:**
```cpp
factorInt(0)  // Returns 1
factorInt(1)  // Returns 10
factorInt(2)  // Returns 100
```

### Binary Conversion

#### `floatToBytes()`
```cpp
uint8_t* floatToBytes(const float value)
```

Converts a float to its IEEE 754 binary representation as a byte array.

**Parameters:**
- `value`: The float value to convert

**Returns:** Pointer to a static 4-byte array containing the IEEE 754 representation

**Important:** Returns a pointer to a static buffer that is overwritten on each call. If you need to preserve multiple conversions, copy the data immediately.

**Example:**
```cpp
// 123.5 in IEEE 754: 0x42f70000
uint8_t* bytes = floatToBytes(123.5F);
// bytes[0] = 0x00, bytes[1] = 0x00, bytes[2] = 0xf7, bytes[3] = 0x42
```

#### `bytesToFloat()`
```cpp
float bytesToFloat(const uint8_t bytes[4])
```

Converts a 4-byte IEEE 754 representation to a float value.

**Parameters:**
- `bytes`: Pointer to 4-byte array containing IEEE 754 representation

**Returns:** The reconstructed float value

**Example:**
```cpp
uint8_t data[4] = {0x00, 0x00, 0xf7, 0x42};
float value = bytesToFloat(data);  // Returns 123.5
```

## Use Cases in the Project

### 1. Display Formatting

The integer and fractional part functions are used to format sensor measurements for display with controlled decimal precision:

```cpp
// Format temperature with 1 decimal place
float temp = 23.456F;
int32_t tempInt = integerPart(temp, 1);      // 23
uint32_t tempFrac = fractionalPart(temp, 1);  // 5
// Display as "23.5°C"
```

### 2. LoRaWAN Payload Serialization

The byte conversion functions are used in [measurementgroup.cpp](measurementgroup.cpp:35-36) to serialize sensor measurements for transmission:

```cpp
// Serialize measurement value to bytes for LoRaWAN payload
const uint8_t* valueAsBytes = floatToBytes(measurements[i].value);
memcpy(buffer + offset, valueAsBytes, 4);
```

### 3. Data Storage

The byte conversion functions enable efficient storage of float measurements in EEPROM (non-volatile storage).

## Implementation Notes

### Rounding Behavior

The library uses `roundf()` for rounding, which rounds to the nearest integer. For values exactly halfway between two integers (e.g., 0.5), it rounds to the nearest even number (banker's rounding) on most platforms.

### Memory Safety

- `floatToBytes()` uses a static buffer, so the returned pointer is valid only until the next call
- All functions use `std::memcpy` for safe binary copying
- A static assertion ensures float is 4 bytes (IEEE 754 single precision)

### Platform Compatibility

The library works on both:
- **Native platform** (desktop testing): Uses standard C++ float operations
- **STM32 target**: Uses hardware floating-point unit (FPU) when available

## Testing

Comprehensive unit tests are available in [test/generic/test_float/test.cpp](../../test/generic/test_float/test.cpp) covering:
- Integer and fractional part extraction with various decimal precisions
- Positive and negative value handling
- Scaling factor generation
- IEEE 754 binary conversion accuracy

Run tests with:
```bash
pio test -e all_generic_unittests -f test_float
```

## IEEE 754 Reference

The byte conversion functions use IEEE 754 single-precision format (32-bit):
- 1 bit: Sign
- 8 bits: Exponent
- 23 bits: Mantissa

For testing and validation, see: [IEEE 754 Converter](https://www.h-schmidt.net/FloatConverter/IEEE754.html)

## License

[CC BY-NC-SA 4.0](https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode)
