# MuMo SW V3
## ToDo's

1. make sensorSettings persistent
2. make SNR work both in devStatusReq and on display
3. make measurementGroup a template class, so we can make large ones (for sensorDeviceCollection) and small ones (for events such as usb plugin...)
4. make epaper display refresh faster...
5. start adding unit test for main application. eg. all changes for cli commands
6. implement configuring display


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
* Make ePaper display more lowPower
  * use partial update or fast update. Full update once per x updates

* ST HAL
  * remove HAL code from C++ wrapper

* getbatteryChargeFromVoltage table right by doing a discharge test over 24h


* merge hw aes tests into 1, in order to not have too many target unit tests
* make generic unit tests also run on target

* when a certain device/channel is not active, it should not be allowed on the display...