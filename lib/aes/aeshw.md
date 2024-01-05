Mapping aes keys and datablocks to bytes and 32words is a bit tricky, as indexes are confusing :
128bit AES Keys are shown as : "FF EE DD CC BB AA 99 88 77 66 55 44 33 22 11 00", where FF holds the Most Signiciant Digits of the number, 00 the least significant.
When this is stored in a byte array : uint8_t keyBytes[16]{0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00};
keyBytes[0] = 0xFF, keyBytes[0x0F] = 00 -> the Most Significant Digit is in the FIRST byte, which has the LOWEST array index, and is stored in the LOWEST memory address
When transferring it to the AES hardware-peripheral, we need 32bit words.
uint32_t keyWords[4]{
    0xFFEEDDCC,
    0xBBAA9988,
    0x77665544,
    0x33221100};
Note that the bytes are expected big-endian, and so we cannot simply use the compilers conversion between uint32 and uint8[4] as this will be little-endian.
keyWords[0] = 0xFFEEDDCC, keyWords[3] = 0x33221100

Then when using this with the AES HW, the 4 key registers expect the following :
  AES_KEYR3 = keyWords[0]
  AES_KEYR2 = keyWords[1]
  AES_KEYR1 = keyWords[2]
  AES_KEYR0 = keyWords[3]