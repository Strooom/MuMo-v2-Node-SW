# QR Code Library

A lightweight, embedded-friendly QR Code generator library written in C++ for the MuMo-v2-Node firmware.

## Overview

This library generates QR codes suitable for embedded systems where dynamic memory allocation should be avoided. It supports QR code versions 1-5 (configurable at compile time) with all four error correction levels. The library automatically selects the optimal encoding format (numeric, alphanumeric, or byte) based on the input data.

## Features

- **Static memory allocation**: All buffers are allocated at compile time based on maximum version configured
- **Multiple encoding formats**: Automatically selects the most efficient encoding (numeric, alphanumeric, or byte)
- **Error correction**: Supports all four QR code error correction levels (Low, Medium, Quartile, High)
- **Reed-Solomon error correction**: Built-in implementation for robust data recovery
- **Automatic mask selection**: Evaluates all 8 mask patterns and selects the optimal one based on penalty scores
- **Configurable version range**: Compile-time configuration to limit memory usage (default: versions 1-5)
- **Embedded-friendly**: Designed for microcontrollers with limited RAM

## Key Components

### Main Classes

#### `qrCode`
The primary interface for QR code generation. Provides static methods for version calculation, encoding, and rendering.

#### `bitVector<size>`
Template class for efficient bit-level data manipulation. Used to store encoded payload data and error correction bytes.

#### `bitMatrix<size>`
Template class for 2D bit arrays. Stores the final QR code pattern as a matrix of modules (pixels).

#### `reedSolomon`
Implements Reed-Solomon error correction code generation using polynomial arithmetic over GF(256).

### Supporting Classes

- **`gf256.hpp`**: Galois Field GF(256) arithmetic for Reed-Solomon calculations
- **`polynome.hpp`**: Polynomial operations in GF(256)
- **`encodingFormat`**: Enumeration of encoding types (numeric, alphanumeric, byte)

## Usage

### Basic Example

If you know the QR code version at compile time:

```cpp
#include <qrcode.hpp>

// Generate a QR code
const char* data = "Hello World";
uint32_t version = 2;
errorCorrectionLevel ecLevel = errorCorrectionLevel::medium;

qrCode::generate(data, version, ecLevel);

// Read the result pixel by pixel
uint32_t size = qrCode::size();
for (uint32_t y = 0; y < size; y++) {
    for (uint32_t x = 0; x < size; x++) {
        bool isBlack = qrCode::getModule(x, y);
        // Render the module (pixel) as needed
    }
}
```

### Dynamic Version Selection

If the data size is only known at runtime:

```cpp
#include <qrcode.hpp>

const char* data = "https://example.com";

// Find minimum version needed for this data
errorCorrectionLevel minEcLevel = errorCorrectionLevel::medium;
uint32_t version = qrCode::versionNeeded(data, minEcLevel);

if (version == 0) {
    // Data too large for configured maximum version
    return;
}

// Optionally check if higher error correction is possible
errorCorrectionLevel maxEcLevel = qrCode::errorCorrectionLevelPossible(data, version);

// Generate with optimal error correction
qrCode::generate(data, version, maxEcLevel);

// Read the result
uint32_t size = qrCode::size();
for (uint32_t y = 0; y < size; y++) {
    for (uint32_t x = 0; x < size; x++) {
        bool isBlack = qrCode::getModule(x, y);
        // Render...
    }
}
```

### Binary Data Support

The library also supports raw binary data (not just null-terminated strings):

```cpp
uint8_t binaryData[] = {0x01, 0x02, 0x03, 0x04, 0x05};
uint32_t dataLength = sizeof(binaryData);

uint32_t version = qrCode::versionNeeded(binaryData, dataLength, errorCorrectionLevel::low);
qrCode::generate(binaryData, dataLength, version, errorCorrectionLevel::low);
```

## API Reference

### Public API Functions

#### Version Calculation
```cpp
static uint32_t versionNeeded(const char* data, errorCorrectionLevel minimumErrorCorrectionLevel);
static uint32_t versionNeeded(const uint8_t* data, uint32_t dataLength, errorCorrectionLevel minimumErrorCorrectionLevel);
```
Returns the minimum QR code version (1-40) needed to accommodate the data at the specified error correction level. Returns 0 if data cannot fit within the configured maximum version.

#### Error Correction Level Optimization
```cpp
static errorCorrectionLevel errorCorrectionLevelPossible(const char* data, uint32_t someVersion);
static errorCorrectionLevel errorCorrectionLevelPossible(const uint8_t* data, uint32_t dataLength, uint32_t someVersion);
```
Returns the maximum error correction level that can be achieved for the given data and version.

#### QR Code Generation
```cpp
static void generate(const char* data, uint32_t someVersion, errorCorrectionLevel someErrorCorrectionLevel);
static void generate(const uint8_t* data, uint32_t dataLength, uint32_t someVersion, errorCorrectionLevel someErrorCorrectionLevel);
```
Generates the QR code. This function encodes the data, adds error correction, applies optimal masking, and draws all patterns.

#### Result Access
```cpp
static bool getModule(uint32_t x, uint32_t y);
static uint32_t size();
```
- `getModule(x, y)`: Returns true if the module at coordinates (x, y) is black, false if white
- `size()`: Returns the width/height of the generated QR code in modules

## Error Correction Levels

```cpp
enum class errorCorrectionLevel {
    low      = 0,  // ~7% error recovery
    medium   = 1,  // ~15% error recovery
    quartile = 2,  // ~25% error recovery
    high     = 3   // ~30% error recovery
};
```

## Encoding Formats

The library automatically selects the most efficient encoding:

- **Numeric**: For digits 0-9 only (most efficient for numeric data)
- **Alphanumeric**: For 0-9, A-Z, space, and special characters ($%*+-./:)
- **Byte**: For any binary data (8-bit)

## Configuration

The library's memory footprint can be configured at compile time:

```cpp
class qrCode {
    static constexpr uint32_t maxVersion{5};              // Maximum version supported (1-40)
    static constexpr uint32_t maxSize{17 + 4 * maxVersion}; // Calculated max QR code size
    static constexpr uint32_t maxInputLength{127U};       // Max string length for safety
    static constexpr uint32_t maxPayloadLengthInBytes{256U}; // Max payload buffer
    // ...
};
```

### QR Code Versions and Sizes

| Version | Size (modules) | Max Data (Low EC) | Max Data (High EC) |
|---------|---------------|-------------------|-------------------|
| 1 | 21×21 | 19 bytes | 9 bytes |
| 2 | 25×25 | 34 bytes | 16 bytes |
| 3 | 29×29 | 55 bytes | 26 bytes |
| 4 | 33×33 | 80 bytes | 36 bytes |
| 5 | 37×37 | 108 bytes | 46 bytes |

## Coordinate System

```
(0,0) ............. (x,0)
  .                   .
  .                   .
  .                   .
(0,y) ............. (x,y)
```

- Origin (0,0) is at the top-left
- X-axis runs horizontally left to right
- Y-axis runs vertically top to bottom

## Implementation Details

### Encoding Pipeline

1. **Data Analysis**: Determines optimal encoding format (numeric/alphanumeric/byte)
2. **Payload Encoding**: Encodes data with mode indicator, character count, and compressed payload
3. **Padding**: Adds terminator bits, bit padding, and byte padding
4. **Error Correction**: Generates Reed-Solomon error correction bytes using polynomial division
5. **Interleaving**: Interleaves data and error correction blocks (when using multiple blocks)
6. **Pattern Drawing**: Draws finder patterns, timing patterns, alignment patterns, and format information
7. **Payload Placement**: Places encoded data in the QR code matrix following the standard zigzag pattern
8. **Mask Selection**: Evaluates all 8 mask patterns using penalty rules and selects the optimal one

### Mask Pattern Selection

The library automatically evaluates all 8 QR code mask patterns and selects the one with the lowest penalty score. Penalty rules assess:

1. **Penalty 1**: Consecutive modules of the same color in rows/columns
2. **Penalty 2**: 2×2 blocks of the same color
3. **Penalty 3**: Finder-like patterns in data area
4. **Penalty 4**: Deviation from 50% dark/light module ratio

## Memory Usage

For `maxVersion = 5`:
- `maxSize = 37` modules
- `modules` bitMatrix: ~172 bytes (37×37 bits)
- `isData` bitMatrix: ~172 bytes
- `buffer` bitVector: 256 bytes
- **Total**: ~600 bytes of static RAM

## Testing

The library includes comprehensive unit tests located in the test directories. Tests verify:
- Encoding format selection
- Numeric, alphanumeric, and byte encoding
- Reed-Solomon error correction
- Version and size calculations
- Pattern drawing
- Mask selection and penalty calculations

## Credits and References

Based on the work of:
- [Richard Moore - QRCode](https://github.com/ricmoo/QRCode)
- [Nayuki - QR Code generator library](https://www.nayuki.io/page/qr-code-generator-library)

Helpful resources:
- [Thonky QR Code Tutorial](https://www.thonky.com/qr-code-tutorial/)
- [QR Codes: A Visual Explainer](https://amodm.com/blog/2024/05/28/qr-codes-a-visual-explainer)
- [Nayuki - Creating a QR Code step by step](https://www.nayuki.io/page/creating-a-qr-code-step-by-step)

## License

CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/

## Author

Pascal Roobrouck - https://github.com/Strooom
