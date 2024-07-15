// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################
// based on the work of Richard Moore : https://github.com/ricmoo/QRCode
// based on the work of Nayuki : https://www.nayuki.io/page/qr-code-generator-library

#pragma once
#include <stdint.h>

enum class encodingFormat {
    numeric      = 0,
    alphanumeric = 1,
    byte         = 2
};

enum class errorCorrectionLevel {
    low      = 0,
    medium   = 1,
    quartile = 2,
    high     = 3
};

class qrCode {
  public:
    static int max(int a, int b);
    static int8_t getAlphanumeric(char c);
    static bool isAlphanumeric(const char *text, uint16_t length);
    static bool isNumeric(const char *text, uint16_t length);
    static uint16_t bb_getGridSizeBytes(uint8_t size);
    static uint16_t bb_getBufferSizeBytes(uint32_t bits);
    // static void bb_setBit(BitBucket *bitGrid, uint8_t x, uint8_t y, bool on);
    // static void bb_invertBit(BitBucket *bitGrid, uint8_t x, uint8_t y, bool invert);
    // static bool bb_getBit(BitBucket *bitGrid, uint8_t x, uint8_t y);

#ifndef unitTesting

  private:
#endif
};

// If set to non-zero, this library can ONLY produce QR codes at that version
// This saves a lot of dynamic memory, as the codeword tables are skipped
#ifndef LOCK_VERSION
#define LOCK_VERSION 0
#endif

typedef struct QRCode {
    uint8_t version;
    uint8_t size;
    uint8_t ecc;
    uint8_t mode;
    uint8_t mask;
    uint8_t *modules;
} QRCode;

uint16_t qrcode_getBufferSize(uint8_t version);
int8_t qrcode_initText(QRCode *qrcode, uint8_t *modules, uint8_t version, uint8_t ecc, const char *data);
int8_t qrcode_initBytes(QRCode *qrcode, uint8_t *modules, uint8_t version, uint8_t ecc, uint8_t *data, uint16_t length);
bool qrcode_getModule(QRCode *qrcode, uint8_t x, uint8_t y);
