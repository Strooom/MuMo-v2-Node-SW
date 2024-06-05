// This implements a large circular buffer to store measurements in EEPROM
// All measurements are stored in a measurementGroup, which is a set of measurements with the same timestamp.
// groupFormat is :
// byte 0 : number of measurements in the group
// byte 1-4 : timestamp
// byte 5-n : measurements
// measurementFormat is :
// byte 0 : header, consisting of deviceIndex and channelIndex
// byte 1-4 : value as float

// in RAM we keep a newMeasurementsOffset pointing an offset in the measurements section of the EEPROM where the next data should be written.
// After writing a batch of data, we also write some (eg.4) bytes of 0xFF. This measurementGap is used to restore the offsets after a reset

// When writing data to eeprom, we need to take care of crossing page boundaries, 128 or 256 bytes depending on the type of EEPROM -> maybe we need a setting for the page size.
// if there is a page boundary between start and end-address, we need to split it to start - boundary and boundary - end.

// search the end of measurements by searching for 0xFF, 0xFF, 0xFF, 0xFF
// If the bytes after that are not 0xFF, older measurements are overwritten and the start of measurements is after the 0xFF, 0xFF, 0xFF, 0xFF
// If the bytes are 0xFF, 0xFF, 0xFF, 0xFF, the start of measurements is at offset 0


saveNewMeasurementsToEeprom : needs wrapping

I could add some logging to show the number of eeprom measurements, as an overview, not all the data..

after writing into eepromm I could leave them in newMeasurements buffer to transmit from there..