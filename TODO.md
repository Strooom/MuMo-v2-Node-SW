# MuMo SW V3
## ToDo's

### Priority High

* VUSB
detect rising edge interrupt and wake mcu up immediately

* measurementsCollection
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

* report number of measurements on bootscreen
* merge hw aes tests into 1, in order to not have too many target unit tests
* make generic unit tests also run on target

### Done

* CLI : this allow to configure the MuMo-V3 / MuMo-V3-mini Hardware after manufacturing
  * provision DevAddr and LoRaWAN keys
  * set batteryType and radioType
  * set sensor parameters
  * set display parameters
  * reading stored measurements from device to computer

* Put some of the aesKey stuff, only needed for SW-AES in conditional compilation, eg key expansion
* clean up / finalize QR-code class
* read LoRaWAN properties in a safe way to display them in mainController.. I need some getters() io making them public



