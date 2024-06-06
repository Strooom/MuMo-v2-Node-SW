# TODO Features

* get LowPower working again
* extend SX126x code to the 14dBm veriant of the Wio-E5
* extend stateMachine : if no LoRaWAN config found -> show msg asking for config
* stateMachine :
 - first screen after boot : show 
   - version info
   - hw info : sensors found
   - lorawan config ok/nok
 - then do lora request for time and network quality, show on second screen
 - then go to normal measuring

* make EEPROM work for multiples of 64K : 
  - I2C address = 0x50 + address/64K
  - address = address % 64K

* report battery charging state to LoRaWAN deviceStatusReq
* make display show non-blocking
* reading the tamper button and wake up from it
* get cli working
* take channel-initialization out of the device initialization and into sensorDeviceCollection
* allow firmware upgrade from usb/serial


# TODO Bugs / Issues
* eerste LUX meting na power on is fout 16700 lux ???
* batteryVoltage seems incorrect ??
* get networkStatus for showing on display and reporting to LNS
* wrapping of measurementsCollection : writing and reading measurements
* erase old measurements when space needed for new ones
* target unit test for wait time after page write into eeprom


# TODO DevOps

* make generic unit tests also run on target
* merge hw aes tests into 1, in order to not have too many target unit tests
* Put some of the aesKey stuff, only needed for SW-AES in conditional compilation, eg key expansion
