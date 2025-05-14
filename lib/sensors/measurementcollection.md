# MeasurementCollection
= a collection of previous measurements

The device stores all measurements in EEPROM, so they can be read through the CLI, or can be retransmitted in case the backend application detects the loss of measurements:
* an occasional packet loss results in a missing uplinkFrameCount.
* during travel of the sensorNode, it may be out of reach of a gateway for some time, resulting in multiple uplink packets lost.

The device can have different size of EEPROM, depending on the type and manufacturing:
* the regular sensorNode has 1 to 4 BR24G1MFVT devices, with 128Kbyte each.
* the mini sensorNode has 1 BR24G1MFVT device.

Whatever the amount of EEPROM available, it also needs to store other information (device config and network config), so the first 4K are reserved for this.
The remainder amount of memory is used for storing measurements.

Design challenges:
1. each sensor can have different sampling parameters: eg batteryVoltage is sampled once per day, temperature is sampled every 10 minutes. As such, a group of measurements does not have a fixed size, but rather a variable size.
2. due to the limited number of write-cycles of an EEPROM device, it is not possible to keep pointers to the measurements that change on every storage of new measurements. So these pointers are kept in RAM, and after power-on, the device will need to scan the EEPROM and determine the locations of measurements, in order to find the location where it can store the new ones.
3. after some time the allocated EEPROM will be full, and from then on, the oldest measurements will be overwritten by new ones.
4. when writing to EEPROM, it is much faster to write a 'page' rather than byte per byte.. (a 3.5 ms delay is needed after every write). However writing pages has an extra complexity that those page-writes cannot cross page boundaries. Page boundaries are HW dependent, 128 or 256 bytes. So when writing a measurementGroup, the application needs to check for page boundaries and if needed split into two writes.

## MeasurementGroup 
a set of measurements with the same timestamp.
* byte 0 : number of measurements in the group
* byte 1-4 : timestamp (32-bit unix epoch)
* byte 5-n : measurements

## Measurement
* byte 0 : header, consisting of deviceIndex and channelIndex
* byte 1-4 : value as float

## measurementsCollection metadata (RAM)
* newMeasurementsOffset: offset in the measurements section of the EEPROM where the next data should be written.
* oldestMeasurementOffset: offset in the measurements section of the EEPROM where the oldest data was written.
* measurementGap: in order to be able to find these two offsets, there needs to be some 'gap' between them, which is filled with 0xFF bytes. 0xFF is the value of blank EEPROM. Inside measurementGroups, 4 consecutive 0xFF bytes can never appear(*), so this gap needs to be at least 4 bytes. Whenever overwriting old measurements, the old measurement will be completely overwritten with 0xFF bytes.
(*) 0xFFFFFFFF is not a valid value for either the timestamp of a float measurement value

## discovery of measurementsCollection Strategy
There are 2 possible scenarios
1. the EEPROM is empty / there are zero measurements
2. the EEPROM is not empty / there are >0 measurements

Let's look at scenario 2 first: there is a gap between the oldest and newest measurement, we need to find this gap
This gap can be in the middle of the memory, or wrapped around the end/beginning.
In order to reliably find the start/end of the gap for both cases, we use the following strategy:
1. search the gap start
    1.1 search backward from the end for a non-0xFF byte
    1.2 search forward from the first non-0xFF byte for a gap = 4 consecutive 0xFF bytes. This search may wrap around the memory end/start
2. search the gap end
    2.1 search forward from the start for a non-0xFF byte
    2.2 search backward from this first non-0xFF byte for a gap = 4 consecutive 0xFF bytes. This search may wrap around the memory start/end

if no non-0xFF byte is found, the memory is still blank
if non-0xFF byte can be found, but no gap can be found, the memory is corrupt...

