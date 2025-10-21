# Sensors Library

This library provides comprehensive sensor management for the MuMo environmental monitoring device, implementing automatic discovery, flexible sampling control, data aggregation, and persistent storage of measurements.

## Architecture Overview

The sensors library follows a layered architecture:

```
┌─────────────────────────────────────────────────┐
│      sensorDeviceCollection (Management)        │
├─────────────────────────────────────────────────┤
│  Individual Sensor Drivers (BME680, SHT40, ...) │
├─────────────────────────────────────────────────┤
│       sensorChannel (Sampling Control)          │
├─────────────────────────────────────────────────┤
│  measurementGroup + measurementGroupCollection  │
│              (Data Aggregation)                 │
└─────────────────────────────────────────────────┘
```

## Core Components

### sensorDeviceCollection

Static class that manages all sensor devices in the system.

**Key responsibilities:**
- Automatic I2C bus scanning and device discovery
- Coordinating sampling across all sensors
- Aggregating measurements from multiple sensors
- Managing sensor configuration and state

**Key methods:**
- `discover()`: Scans I2C bus for connected sensors and initializes them
- `startSampling()`: Triggers sampling for all active sensors
- `run()`: State machine execution for all sensors
- `isSamplingReady()`: Checks if all sensors have completed sampling
- `collectNewMeasurements()`: Gathers new measurements into a measurementGroup
- `set(deviceIndex, channelIndex, oversamplingIndex, prescalerIndex)`: Configure channel sampling

**Supported sensor devices:**
- Battery voltage and state of charge
- BME680: Temperature, humidity, barometric pressure (and gas resistance)
- SHT40: Temperature and humidity
- TSL2591: Light intensity
- SCD40: CO2, temperature, humidity
- SPS30: Particulate matter (PM1.0, PM2.5, PM4.0, PM10)
- MCU: Radio type information

Each sensor device is identified by `sensorDeviceType` enum and tracked in the `present[]` array.

### sensorChannel

Represents a single measurement channel with intelligent sampling control.

**Key features:**
- **Oversampling**: Averages multiple samples to reduce noise
- **Prescaling**: Controls output frequency (how often to store measurements)
- **State tracking**: Maintains counters to determine when to sample vs output

**Sampling control:**
- Oversampling: Averages 1, 2, 4, or 10 samples
- Prescaling: Output interval from 1 minute to 2 days
- Configured via lookup tables to reduce storage requirements

**Key methods:**
- `setIndex(oversamplingIndex, prescalerIndex)`: Configure channel behavior
- `needsSampling()`: Returns true when new sample is needed
- `hasOutput()`: Returns true when averaged value is ready
- `addSample(value)`: Add a new raw sample
- `value()`: Get averaged measurement value
- `updateCounters()`: Advance state machine (called every RTC tick)

**Example usage:**
```cpp
// Configure channel: 10 samples averaged, 10 minute output interval
channel.setIndex(3, 4); // oversamplingIndex 3 = 10 samples, prescalerIndex 4 = 20 ticks = 10 minutes

// During measurement cycle
if (channel.needsSampling()) {
    float sample = readSensor();
    channel.addSample(sample);
}

if (channel.hasOutput()) {
    float averaged = channel.value();
    // Store or transmit averaged measurement
}

// Called every 30 seconds by RTC tick
channel.updateCounters();
```

### measurementGroup

Container for a timestamped collection of measurements from multiple sensors.

**Structure:**
- Timestamp (Unix time_t)
- Array of up to 16 measurements
- Each measurement: deviceIndex, channelIndex, float value
- XOR checksum for data integrity

**Key methods:**
- `addMeasurement(deviceIndex, channelIndex, value)`: Add a measurement
- `setTimestamp(timestamp)`: Set the measurement timestamp
- `toBytes(buffer, bufferSize)`: Serialize for storage or transmission
- `fromBytes(source)`: Deserialize from storage
- `getNumberOfMeasurements()`: Get count of measurements in group

**Binary format:**
```
Byte 0:       Number of measurements (N)
Bytes 1-4:    Timestamp (32-bit Unix time)
Bytes 5...:   N × 5 bytes per measurement:
              - 1 byte: compressed device+channel index
              - 4 bytes: float value (IEEE 754)
Last byte:    XOR checksum
```

### measurementGroupCollection

Manages circular buffer storage of measurement groups in non-volatile memory (EEPROM).

**Key features:**
- Circular buffer with oldest/newest offsets
- Automatic wrapping when storage is full
- Checksum validation on read
- Integration with NVS (non-volatile storage) layer

**Key methods:**
- `initialize()`: Read storage pointers from EEPROM
- `addNew(measurementGroup)`: Store new measurement group
- `get(measurementGroup, offset)`: Retrieve measurement group
- `eraseOldest()`: Remove oldest measurement to free space
- `getFreeSpace()`: Get available storage capacity

See [lib/nvs/README.md](../nvs/README.md) for details on EEPROM organization and page-aware operations.

## Individual Sensor Drivers

Each sensor driver implements a common interface pattern:

**Common structure:**
- Static class (no instantiation)
- State machine with `sensorDeviceState` enum
- Array of `sensorChannel` objects (one per measurement type)
- Constant `nmbrChannels` defining channel count

**Common methods:**
- `isPresent()`: Check if sensor is connected via I2C
- `initialize()`: Configure sensor for operation
- `startSampling()`: Trigger new measurement
- `run()`: State machine execution
- `getState()`: Return current sensor state

**Example: BME680 driver**
```cpp
class bme680 {
  public:
    static constexpr uint32_t nmbrChannels{3};
    static bool isPresent();
    static void initialize();
    static void startSampling();
    static void run();
    static sensorDeviceState getState();

  private:
    static sensorDeviceState state;
    static sensorChannel channels[nmbrChannels];
    // Calibration coefficients, raw data, etc.
};
```

### Battery Monitoring

Special sensor that measures battery voltage and calculates state of charge.

**Channels:**
- Voltage (V, 2 decimals)
- State of charge (%, 2 decimals)

**Key features:**
- Configurable battery chemistry (LiFePO4, Li-Ion, etc.)
- Voltage-to-charge curve interpolation
- ADC-based voltage measurement

### BME680

Environmental sensor from Bosch Sensortec.

**Channels:**
- Temperature (°C, 1 decimal)
- Relative humidity (%, 0 decimals)
- Barometric pressure (hPa, 0 decimals)

**Features:**
- Factory calibration coefficients
- I2C communication at 0x76 or 0x77
- Multi-step state machine for sampling

### SHT40

High-precision temperature and humidity sensor from Sensirion.

**Channels:**
- Temperature (°C)
- Relative humidity (%)

### TSL2591

High-dynamic-range light sensor from AMS.

**Channels:**
- Light intensity (lux)

**Features:**
- Wide dynamic range
- Integration time and gain control

### SCD40

CO2 sensor from Sensirion with integrated temperature and humidity.

**Channels:**
- CO2 concentration (ppm)
- Temperature (°C)
- Relative humidity (%)

**Features:**
- True CO2 sensor (NDIR principle)
- Automatic self-calibration

### SPS30

Particulate matter sensor from Sensirion.

**Channels:**
- PM1.0, PM2.5, PM4.0, PM10 mass concentrations

**Features:**
- Laser-based particle detection
- Long lifetime and stability

## Sensor States

All sensor drivers use the `sensorDeviceState` enum:

- `unknown`: Initial state
- `unpowered`: Sensor is powered off
- `sleeping`: Idle, ready to sample
- `standby`: Preparing for measurement
- `sampling`: Actively measuring

## Configuration and Persistence

Sensor configurations are persisted in EEPROM via the settings system:

**Per-channel settings:**
- Oversampling index (0-3)
- Prescaler index (0-13)
- Index 0 = disabled channel

**Access via settingsCollection:**
```cpp
// Get channel config
uint8_t config = settingsCollection::read(settingIndex);
uint32_t oversamplingIndex = sensorChannel::extractOversamplingIndex(config);
uint32_t prescalerIndex = sensorChannel::extractPrescalerIndex(config);

// Set channel config
uint8_t newConfig = sensorChannel::compressOversamplingAndPrescalerIndex(
    oversamplingIndex, prescalerIndex);
settingsCollection::write(settingIndex, newConfig);
```

## Usage Workflow

**1. Initialization (boot time):**
```cpp
sensorDeviceCollection::discover();  // Scan I2C bus, initialize found sensors
```

**2. Configuration (via CLI or defaults):**
```cpp
// Configure battery voltage: no oversampling, 10 minute interval
sensorDeviceCollection::set(
    static_cast<uint32_t>(sensorDeviceType::battery),
    battery::voltage,
    0,  // oversamplingIndex: 1 sample
    4   // prescalerIndex: 10 minutes
);
```

**3. Periodic sampling (every RTC tick, 30 seconds):**
```cpp
// Check if any sensors need sampling
if (sensorDeviceCollection::needsSampling()) {
    sensorDeviceCollection::startSampling();

    // Run state machines until complete
    while (!sensorDeviceCollection::isSamplingReady()) {
        sensorDeviceCollection::run();
    }
}

// Update counters for next cycle
sensorDeviceCollection::updateCounters();
```

**4. Data collection and storage:**
```cpp
// Check if new measurements are ready
if (sensorDeviceCollection::hasNewMeasurements()) {
    sensorDeviceCollection::collectNewMeasurements();

    // Access collected measurements
    measurementGroup& measurements = sensorDeviceCollection::newMeasurements;

    // Store in EEPROM
    measurementGroupCollection::addNew(measurements);

    // Or prepare for LoRaWAN transmission
    // (see mainController for integration)
}
```

## Integration Points

**With mainController:**
- `sampling` state: Runs sensor sampling workflow
- `networking` state: Transmits collected measurements via LoRaWAN

**With display:**
- Display lines can show any sensor channel
- Mapping configured via display settings
- Values formatted with appropriate decimals and units

**With LoRaWAN:**
- Payload builder selects channels to transmit
- Binary encoding reduces airtime
- Uplink messages contain compressed measurement data

**With CLI:**
- Status commands show all sensor values
- Configuration commands set channel parameters
- Export commands retrieve stored measurements

## Adding a New Sensor

To add a new sensor device:

1. **Create header and implementation files** (e.g., `newsensor.hpp`, `newsensor.cpp`)

2. **Define the sensor class** following the common pattern:
```cpp
class newSensor {
  public:
    static constexpr uint32_t nmbrChannels{2};
    static bool isPresent();
    static void initialize();
    static void startSampling();
    static void run();
    static sensorDeviceState getState();

  private:
    static sensorDeviceState state;
    static sensorChannel channels[nmbrChannels];
};
```

3. **Add to sensorDeviceType enum** in [sensordevicetype.hpp](sensordevicetype.hpp):
```cpp
enum class sensorDeviceType : uint32_t {
    // ... existing devices ...
    newSensor,
    nmbrOfKnownDevices
};
```

4. **Update sensorDeviceCollection** in [sensordevicecollection.cpp](sensordevicecollection.cpp):
   - Add include for new sensor
   - Add discovery code to `discover()`
   - Add cases to `startSampling()`, `run()`, `isSamplingReady()`, `nmbrOfChannels()`, `channel()`, `name()`

5. **Update settings** if persistent configuration is needed

6. **Write unit tests** for the new sensor driver

## Testing

**Generic tests** (native platform):
- `test/generic/test_sensorchannel/`: Channel sampling logic
- `test/generic/test_measurementgroup/`: Measurement serialization
- Mocked I2C for sensor driver testing

**Target tests** (hardware required):
- `test/target_no_jigs/test_sensors/`: Real sensor hardware tests
- Validates I2C communication and sensor responses

Run tests:
```bash
pio test -e all_generic_unittests              # Native tests
pio test -e all_target_mumo_v2x_unittests      # Hardware tests
```

## Design Rationale

**Why static classes?**
- Single instance of each sensor type
- Reduces memory overhead
- Simplifies access from mainController

**Why separate channels from devices?**
- Some sensors provide multiple measurement types
- Each channel can have independent sampling configuration
- Enables flexible data aggregation and transmission

**Why oversampling and prescaling?**
- Oversampling: Reduces noise without complex filtering
- Prescaling: Optimizes storage and transmission (don't store/send every sample)
- Decouples sampling rate from output rate

**Why measurement groups?**
- Atomic storage of related measurements
- Timestamp synchronization
- Efficient serialization for EEPROM and LoRaWAN

## File Organization

```
lib/sensors/
├── README.md                           # This file
├── todo.md                             # Development tasks
│
├── sensordevicecollection.hpp/cpp      # Central sensor management
├── sensorchannel.hpp/cpp               # Per-channel sampling control
├── measurementgroup.hpp/cpp            # Timestamped measurement container
├── measurementgroupcollection.hpp/cpp  # EEPROM storage management
│
├── sensordevicetype.hpp/cpp            # Sensor type enumeration
├── sensordevicestate.hpp/cpp           # Sensor state machine states
│
├── battery.hpp/cpp                     # Battery monitoring
├── batterytype.hpp/cpp                 # Battery chemistry types
├── chargefromvoltage.hpp/cpp           # Battery charge calculation
│
├── bme680.hpp/cpp                      # BME680 environmental sensor
├── sht40.hpp/cpp                       # SHT40 temp/humidity sensor
├── tsl2591.hpp/cpp                     # TSL2591 light sensor
├── scd40.hpp/cpp                       # SCD40 CO2 sensor
├── sps30.hpp/cpp                       # SPS30 particulate sensor
│
└── transmitpolicy.hpp                  # LoRaWAN transmission policy
```

## References

- BME680 datasheet: [Bosch Sensortec](https://www.bosch-sensortec.com/products/environmental-sensors/gas-sensors/bme680/)
- SHT40 datasheet: [Sensirion](https://www.sensirion.com/en/environmental-sensors/humidity-sensors/humidity-sensor-sht4x/)
- TSL2591 datasheet: [AMS](https://ams.com/tsl25911)
- SCD40 datasheet: [Sensirion](https://www.sensirion.com/en/environmental-sensors/carbon-dioxide-sensors/carbon-dioxide-sensor-scd4x/)
- SPS30 datasheet: [Sensirion](https://www.sensirion.com/en/environmental-sensors/particulate-matter-sensors-pm25/)
- Main project: [MuMo v2 Node GitHub](https://github.com/Strooom/MuMo-v2-Node-SW)
