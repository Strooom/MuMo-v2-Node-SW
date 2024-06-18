# Short Term ToDo's
* deviceStatusReq doen werken met echte waarden
  - ontvangstwaarden van laatste downlink opslaan
  - batterijspanning omzetten in uint8
* batteryVoltage seems incorrect ?? also shown with 1 decimal io 2 ??
* wrapping of measurementsCollection : writing and reading measurements
* erase old measurements when space needed for new ones




# Medium Term ToDo's
* move lptim out of lorawan and make it available to application
* allow firmware upgrade from usb/serial
* show usb connection on display
* improve isModified in display refresh : set to false after updating display, set to true when writing different text into screen
* measurementsScreen only has 3 lines io 4
* high lux value does not show on display







# Long Term ToDo's
* make generic unit tests also run on target
* OTAA
* make EEPROM work for multiples of 64K : 
  - I2C address = 0x50 + address/64K
  - address = address % 64K
* target unit test for wait time after page write into eeprom
* reading the tamper button and wake up from it
* get cli working
* report number of measurements on bootscreen
* merge hw aes tests into 1, in order to not have too many target unit tests
* Put some of the aesKey stuff, only needed for SW-AES in conditional compilation, eg key expansion
* read LoRaWAN properties in a safe way to display them in mainController.. I need some getters() io making them public
 - frameCount
 - devAddr
 - keys
