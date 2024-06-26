# Short Term ToDo's
* show UID as QRCode when LoRaWAN is not yet configured...
* LoRaWAN : investigate why sometimes downlinks are not working : tune Rx timing
* CLI : 
  - UART2 activeren bij USB connectie


* deviceStatusReq doen werken met echte waarden
  - ontvangstwaarden van laatste downlink opslaan
  - batterijspanning omzetten in uint8
* batteryVoltage seems incorrect ?? also shown with 1 decimal io 2 ??
* wrapping of measurementsCollection : writing and reading measurements
* erase old measurements when space needed for new ones
* Give the node a name property which can be set from CLI / downlink, and which appears on the display status bar
* high lux value does not show on display




# Medium Term ToDo's
* Clock synchroniseren 1 maal per week,
* Display updaten minimaal 1 maal per dag
* integrate qrCode as full static class. 





# Long Term ToDo's
* make generic unit tests also run on target
* OTAA
* make EEPROM work for multiples of 64K : 
  - I2C address = 0x50 + address/64K
  - address = address % 64K
* target unit test for wait time after page write into eeprom
* reading the tamper button and wake up from it
* report number of measurements on bootscreen
* merge hw aes tests into 1, in order to not have too many target unit tests
* Put some of the aesKey stuff, only needed for SW-AES in conditional compilation, eg key expansion
* read LoRaWAN properties in a safe way to display them in mainController.. I need some getters() io making them public
 - frameCount
 - devAddr
 - keys
