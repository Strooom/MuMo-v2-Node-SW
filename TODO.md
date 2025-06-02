# MuMo SW V3
## ToDo's
* CLI : this allow to configure the MuMo-V3 / MuMo-V3-mini Hardware after manufacturing
  * provision DevAddr and LoRaWAN keys
  * set batteryType and radioType
  * set sensor parameters
  * set display parameters
  * reading stored measurements from device to computer

* Make ePaper display more lowPower
  * use partial update or fast update. Full update once per x updates

* deviceStatusReq doen werken met echte waarden
  - ontvangstwaarden van laatste downlink opslaan
  - batterijspanning omzetten in uint8 : omrekening toevoegen aan battery

* measurementsCollection
  * make it work for variable amount of EEPROM memory
  * target unit test for wait time after page write into eeprom

* sw quality metrics
  * clean up / finalize QR-code class
  * unit tests for application
  * make generic unit tests also run on target
  * read LoRaWAN properties in a safe way to display them in mainController.. I need some getters() io making them public
    - frameCount
    - devAddr
    - keys

* getbatteryChargeFromVoltage table right by doing a discharge test over 24h

* OTAA ??

* report number of measurements on bootscreen
* merge hw aes tests into 1, in order to not have too many target unit tests
* Put some of the aesKey stuff, only needed for SW-AES in conditional compilation, eg key expansion

# ST HAL
  * remove HAL code from C++ wrapper


# Power events
when connecting / disconnection USB, this should be logged as an measurement-event and transmitted in the measurements

# logging
make logging to uart use interrupts io blocking

# VUSB
detect rising edge interrupt and wake mcu up immediately