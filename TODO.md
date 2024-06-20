# Short Term ToDo's
* USB icon tonen bij USB connectie
* UART2 activeren bij USB connectie
* bootloader jumpen bij detectie 0xF7 van STM32CubeProgrammer
* deviceStatusReq doen werken met echte waarden
  - ontvangstwaarden van laatste downlink opslaan
  - batterijspanning omzetten in uint8
* batteryVoltage seems incorrect ?? also shown with 1 decimal io 2 ??
* wrapping of measurementsCollection : writing and reading measurements
* erase old measurements when space needed for new ones
* show UID as QRCode when LoRaWAN is not yet configured...



# Medium Term ToDo's
* improve isModified in display refresh : set to false after updating display, set to true when writing different text into screen
* measurementsScreen only has 3 lines io 4
* high lux value does not show on display
* Give the node a name property which can be set from downlink, and which appears on the display status bar






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
