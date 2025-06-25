HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);

void nonVolatileStorage::write(const uint32_t startAddress, const uint8_t* sourceData, const uint32_t dataLength)

uint8_t* pData{const_cast<uint8_t*>(sourceData)};

