// Notes on the SX126x to STM32WLE interface
// The SX126x RESET is connected to RCC_CSR:15 (RFRST) for driving it, and RCC_CSR:14 (RFRSTF) for reading it
// The SX126x BUSY is connected to PWR_SR2:1 (RFBUSY)
// The SX126x SPI NSS
// * the functionality is enabled by PWR_CR1:3 (SUBGHZSPINSSSEL)
// * the NSS is driven by PWR_SUBGHZSPICR:14 (NSS)
/*----------------------- SPI CR1 Configuration ----------------------------*
 *             SPI Mode: Master                                             *
 *   Communication Mode: 2 lines (Full-Duplex)                              *
 *       Clock polarity: Low                                                *
 *                phase: 1st Edge                                           *
 *       NSS management: Internal (Done with External bit inside PWR        *
 *  Communication speed: BaudratePrescaler                             *
 *            First bit: MSB                                                *
 *      CRC calculation: Disable                                            *
 *--------------------------------------------------------------------------*/
// constexpr uint32_t MSTR{2};
// SPI_SX126X_CR1.setBit(MSTR);        // All other bits are 0 and this is what we need
/*----------------------- SPI CR2 Configuration ----------------------------*
 *            Data Size: 8bits                                              *
 *              TI Mode: Disable                                            *
 *            NSS Pulse: Disable                                            *
 *    Rx FIFO Threshold: 8bits                                              *
 *--------------------------------------------------------------------------*/
// constexpr uint32_t FRXTH{12};
// SPI_SX126X_CR2.setBit(FRXTH);
//  SPI_SX126X_CR2.writeBits(0xF << 8, 0x7 << 8); // 8 bits data size is already the default
// constexpr uint32_t SPE{6};
// SPI_SX126X_CR1.setBit(SPE); /* Enable SUBGHZSPI Peripheral */
// peripheralRegister SPI_SX126X_CR1(reinterpret_cast<volatile uint32_t *const>(SUBGHZSPI_BASE + 0x00));
// peripheralRegister SPI_SX126X_CR2(reinterpret_cast<volatile uint32_t *const>(SUBGHZSPI_BASE + 0x04));
// peripheralRegister SPI_SX126X_SR(reinterpret_cast<volatile uint32_t *const>(SUBGHZSPI_BASE + 0x08));
// peripheralRegister SPI_SX126X_DR(reinterpret_cast<volatile uint32_t *const>(SUBGHZSPI_BASE + 0x0C));
// peripheralRegister SPI_SX126X_CRCPR(reinterpret_cast<volatile uint32_t *const>(SUBGHZSPI_BASE + 0x10));
// peripheralRegister SPI_SX126X_RXCRCR(reinterpret_cast<volatile uint32_t *const>(SUBGHZSPI_BASE + 0x14));
// peripheralRegister SPI_SX126X_CTXCRCR(reinterpret_cast<volatile uint32_t *const>(SUBGHZSPI_BASE + 0x18));
// extern peripheralRegister RCC_APB3ENR;
// extern peripheralRegister RCC_CSR;
// extern peripheralRegister PWR_SR2;