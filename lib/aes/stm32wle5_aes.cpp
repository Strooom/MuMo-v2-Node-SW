#include <stm32wle5_aes.hpp>
#ifndef generic
#include <main.h>
#endif

// #define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

void stm32wle5_aes::initialize(aesMode theMode) {
    uint32_t aesControlRegisterValue{0};
    aesControlRegisterValue |= (0b10 << 1);                                  // set dataTypeSelection to byte, for bigEndian / littleEndian conversion
    aesControlRegisterValue |= (static_cast<uint32_t>(theMode) << 5);        // set ECB, CBC or CTR MODE

#ifndef generic
    __HAL_RCC_AES_CLK_ENABLE();
    AES->CR   = aesControlRegisterValue;
    AES->IVR0 = 0;
    AES->IVR1 = 0;
    AES->IVR2 = 0;
    AES->IVR3 = 0;
#endif
}

void stm32wle5_aes::enable() {
#ifndef generic
    AES->CR |= AES_CR_EN;
#endif
}
void stm32wle5_aes::disable() {
#ifndef generic
    AES->CR &= ~AES_CR_EN;
#endif
}

void stm32wle5_aes::setKey(aesKey& theKey) {
#ifndef generic
    AES->KEYR0 = aesKey::swapLittleBigEndian(theKey.asWords()[3]);        // LeastSignificant bits
    AES->KEYR1 = aesKey::swapLittleBigEndian(theKey.asWords()[2]);
    AES->KEYR2 = aesKey::swapLittleBigEndian(theKey.asWords()[1]);
    AES->KEYR3 = aesKey::swapLittleBigEndian(theKey.asWords()[0]);        // MostSignificant bits
#endif
}

void stm32wle5_aes::setInitializationVector(aesBlock& theBlock) {
#ifndef generic
    AES->IVR0 = aesBlock::swapLittleBigEndian(theBlock.asWords()[3]);
    AES->IVR1 = aesBlock::swapLittleBigEndian(theBlock.asWords()[2]);
    AES->IVR2 = aesBlock::swapLittleBigEndian(theBlock.asWords()[1]);
    AES->IVR3 = aesBlock::swapLittleBigEndian(theBlock.asWords()[0]);
#endif
}

void stm32wle5_aes::write(aesBlock& theBlock) {
#ifndef generic
    AES->DINR = theBlock.asWords()[0];
    AES->DINR = theBlock.asWords()[1];
    AES->DINR = theBlock.asWords()[2];
    AES->DINR = theBlock.asWords()[3];
#endif
}

void stm32wle5_aes::read(aesBlock& theBlock) {
#ifndef generic
    theBlock.asWords()[0] = AES->DOUTR;
    theBlock.asWords()[1] = AES->DOUTR;
    theBlock.asWords()[2] = AES->DOUTR;
    theBlock.asWords()[3] = AES->DOUTR;
#endif
}

void stm32wle5_aes::write(uint32_t* wordsIn) {
#ifndef generic
    AES->DINR = wordsIn[0];
    AES->DINR = wordsIn[1];
    AES->DINR = wordsIn[2];
    AES->DINR = wordsIn[3];
#endif
}

void stm32wle5_aes::read(uint32_t* wordsOut) {
#ifndef generic
    wordsOut[0] = AES->DOUTR;
    wordsOut[1] = AES->DOUTR;
    wordsOut[2] = AES->DOUTR;
    wordsOut[3] = AES->DOUTR;
    AES->CR |= CRYP_CCF_CLEAR;
#endif
}

bool stm32wle5_aes::isComputationComplete() {
#ifndef generic
    return ((AES->SR & AES_SR_CCF) != 0);
#else
    return true;
#endif
}

void stm32wle5_aes::clearComputationComplete() {
#ifndef generic
    AES->CR |= CRYP_CCF_CLEAR;
#endif
}
