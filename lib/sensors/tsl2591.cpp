#include <tsl2591.hpp>
#include <settingscollection.hpp>
#include <logging.hpp>

#ifndef generic
#include <main.h>
extern I2C_HandleTypeDef hi2c2;
#else
#include <cstring>
extern uint8_t mockTSL2591Registers[256];
#endif

sensorDeviceState tsl2591::state{sensorDeviceState::unknown};
sensorChannel tsl2591::channels[nmbrChannels]{
    {sensorChannelType::TSL25911Infrared},
    {sensorChannelType::TSL25911VisibleLight},
};

tsl2591::integrationTimes tsl2591::integrationTime{integrationTimes::integrationTime100ms};
tsl2591::gains tsl2591::gain{gains::gain1x};

uint32_t tsl2591::rawChannel0{0};
uint32_t tsl2591::rawChannel1{0};

float tsl2591::luxCoefficient{408.0F};
float tsl2591::ch0Coefficient{1.64F};
float tsl2591::ch1Coefficient{0.59F};
float tsl2591::ch2Coefficient{0.86F};

bool tsl2591::isPresent() {
    // 1. Check if something is connected to the I2C bus at the address of the bme680
    if (!testI2cAddress(i2cAddress)) {
        return false;
    }
    // 2. Check if it is a TSL2591 by reading the chip id register/value
    uint8_t chipidValue = readRegister(tsl2591::registers::id);
    return (tsl2591::chipIdValue == chipidValue);
}

void tsl2591::initialize() {
    writeRegister(registers::enable, powerOn);
    setIntegrationTime(integrationTimes::integrationTime100ms);
    setGain(gains::gain428x);

    state = sensorDeviceState::standby;
}

void tsl2591::goSleep() {
    writeRegister(registers::enable, powerOff);
}

void tsl2591::startSampling() {
    writeRegister(registers::enable, 0x03);
    state = sensorDeviceState::sampling;
}

bool tsl2591::samplingIsReady() {
    return (readRegister(registers::status) & 0x01) == 0x01;
}

void tsl2591::readSample() {
    // CHAN0 must be read before CHAN1 See: https://forums.adafruit.com/viewtopic.php?f=19&t=124176
    uint32_t raw1 = readRegister(registers::c0datah);
    uint32_t raw2 = readRegister(registers::c0datal);
    uint32_t raw3 = readRegister(registers::c1datah);
    uint32_t raw4 = readRegister(registers::c1datal);
    rawChannel0   = (raw1 << 8) + raw2;
    rawChannel1   = (raw3 << 8) + raw4;
}

bool tsl2591::anyChannelNeedsSampling() {
    return (channels[infrared].needsSampling() || channels[visible].needsSampling());
}
void tsl2591::adjustAllCounters() {
    channels[infrared].adjustCounters();
    channels[visible].adjustCounters();
}

float tsl2591::calculateVisibleLight() {
    return 0.0F;
}

float tsl2591::calculateInfraredLight() {
    float integrationTimeFactor = static_cast<float>((static_cast<uint32_t>(integrationTime) + 1) * 100);
    float gainFactor;
    switch (gain) {
        case gains::gain1x:
        default:
            gainFactor = 1.0F;
            break;

        case gains::gain25x:
            gainFactor = 25.0F;
            break;

        case gains::gain428x:
            gainFactor = 428.0F;
            break;

        case gains::gain9876x:
            gainFactor = 9876.0F;
            break;
    }

    float totalFactor = (integrationTimeFactor * gainFactor) / luxCoefficient;
    float lux         = (((float)rawChannel0 - (float)rawChannel1)) * (1.0F - ((float)rawChannel1 / (float)rawChannel0)) / totalFactor;
    return lux;
}

void tsl2591::setIntegrationTime(integrationTimes theIntegrationTime) {
    integrationTime = theIntegrationTime;
    uint8_t control = (static_cast<uint8_t>(integrationTime)) | (static_cast<uint8_t>(gain));
    writeRegister(registers::config, control);
}

void tsl2591::setGain(gains theGain) {
    gain            = theGain;
    uint8_t control = (static_cast<uint8_t>(integrationTime)) | (static_cast<uint8_t>(gain));
    writeRegister(registers::config, control);
}

// If the RAW ADC readings are too low, we need to increase the gain or integrationtime, otherwise we would lose precision
// Notes : factors between gains are ~25
// With integration time factors is 6 (600 / 100)

void tsl2591::increaseSensitivity() {
    // If ADC reading is below 2500 (~64K/25), we can increase gain one step.
    // If we are at maximum gain, or reading is above 2500 we may improve the integration time : 
}
void tsl2591::decreaseSensitivity() {
    // When the ADC reading overflow, we first reduce integration time, then gain
}

bool tsl2591::testI2cAddress(uint8_t addressToTest) {
#ifndef generic
    return (HAL_OK == HAL_I2C_IsDeviceReady(&hi2c2, addressToTest << 1, halTrials, halTimeout));
#else
    return true;
#endif
}

uint8_t tsl2591::readRegister(registers registerAddress) {
    uint16_t command = commandMask | static_cast<uint16_t>(registerAddress);
    uint8_t result;

#ifndef generic
    HAL_I2C_Mem_Read(&hi2c2, i2cAddress << 1, command, I2C_MEMADD_SIZE_8BIT, &result, 1, halTimeout);
#else
    result = mockTSL2591Registers[static_cast<uint8_t>(registerAddress)];
#endif
    return result;
}

void tsl2591::writeRegister(registers registerAddress, uint8_t value) {
    uint16_t command = commandMask | static_cast<uint16_t>(registerAddress);
#ifndef generic
    HAL_I2C_Mem_Write(&hi2c2, i2cAddress << 1, command, I2C_MEMADD_SIZE_8BIT, &value, 1, halTimeout);
#else
    mockTSL2591Registers[static_cast<uint8_t>(registerAddress)] = value;
#endif
}

void tsl2591::tick() {
    if (state != sensorDeviceState::sleeping) {
        adjustAllCounters();
        return;
    }

    if (anyChannelNeedsSampling()) {
        startSampling();
        state = sensorDeviceState::sampling;
    } else {
        adjustAllCounters();
    }
}

void tsl2591::run() {
    if ((state == sensorDeviceState::sampling) && samplingIsReady()) {
        readSample();

        // check sensitivity and restart with different settings if needed

        if ((rawChannel0 == 0xFFFF) || (rawChannel1 == 0xFFFF)) {
            decreaseSensitivity();
        }

        if ((rawChannel0 < 0x3333) || (rawChannel1 < 0x3333)) {
            increaseSensitivity();
        }

        if (channels[visible].needsSampling()) {
            float tsl2591visible = calculateVisibleLight();
            channels[visible].addSample(tsl2591visible);
            logging::snprintf(logging::source::sensorData, "%s = %.2f V\n", toString(channels[visible].type), tsl2591visible, postfix(channels[visible].type));

            if (channels[visible].hasOutput()) {
                channels[visible].hasNewValue = true;
            }
        }

        if (channels[infrared].needsSampling()) {
            float tsl2591infrared = calculateInfraredLight();
            channels[infrared].addSample(tsl2591infrared);
            logging::snprintf(logging::source::sensorData, "%s = %.2f V\n", toString(channels[infrared].type), tsl2591infrared, postfix(channels[infrared].type));
            if (channels[infrared].hasOutput()) {
                channels[infrared].hasNewValue = true;
            }
        }

        state = sensorDeviceState::sleeping;
        adjustAllCounters();
    }
}
