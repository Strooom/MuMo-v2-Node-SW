# Short Term ToDo's

* getbatteryChargeFromVoltage table right by doing a discharge test over 24h

* CLI : 
  - UART2 activeren bij USB connectie


* deviceStatusReq doen werken met echte waarden
  - ontvangstwaarden van laatste downlink opslaan
  - batterijspanning omzetten in uint8 : omrekening toevoegen aan battery


* wrapping of measurementsCollection : writing and reading measurements
* erase old measurements when space needed for new ones


# Medium Term ToDo's
* integrate qrCode as full static class. 
  - add unit testing
  - move code from screen to graphics

# Long Term ToDo's

* make generic unit tests also run on target
* OTAA
* make EEPROM work for multiples of 64K : 
  - I2C address = 0x50 + address/64K
  - address = address % 64K
* target unit test for wait time after page write into eeprom

* report number of measurements on bootscreen
* merge hw aes tests into 1, in order to not have too many target unit tests
* Put some of the aesKey stuff, only needed for SW-AES in conditional compilation, eg key expansion
* read LoRaWAN properties in a safe way to display them in mainController.. I need some getters() io making them public
 - frameCount
 - devAddr
 - keys
