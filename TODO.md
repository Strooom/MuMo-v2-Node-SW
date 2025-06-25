# MuMo SW V3
## ToDo's

### Priority High

* measurementsGroupCollection
  * make it work for variable amount of EEPROM memory
  * target unit test for wait time after page write into eeprom

### Priority Medium
* deviceStatusReq doen werken met echte waarden
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