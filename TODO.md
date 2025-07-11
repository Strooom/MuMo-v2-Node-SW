# MuMo SW V3

## known bugs
when unplugging the USB, sometimes the display is not updated immediately, but only at the next RTCtick ???

## ToDo's

2. make SNR work both in devStatusReq and on display
3. make measurementGroup a template class, so we can make large ones (for sensorDeviceCollection) and small ones (for events such as usb plugin...)
5. start adding unit test for main application. eg. all changes for cli commands
7. put MCU in sleep when waiting for low power timer...
8. rework target unit tests to make test-interface uart1 use the same setup

### Priority High

* measurementsGroupCollection
  * make it work for variable amount of EEPROM memory
  * target unit test for wait time after page write into eeprom

### Priority Medium
*  doen werken met echte waarden
  - ontvangstwaarden van laatste downlink opslaan
  - batterijspanning omzetten in uint8 : omrekening toevoegen aan battery

* Power events
when connecting / disconnection USB, this should be logged as an measurement-event and transmitted in the measurements


### Priority Low

* ST HAL
  * remove HAL code from C++ wrapper

* getbatteryChargeFromVoltage table right by doing a discharge test over 24h

* merge hw aes tests into 1, in order to not have too many target unit tests
* make generic unit tests also run on target

* when a certain device/channel is not active, it should not be allowed on the display...

* CLI command to erase all measurements
* CLI command to dump the EEPROM memory