# Short Term ToDo's
* getbatteryChargeFromVoltage table right by doing a discharge test over 24h
* show UID as QRCode
  - on boot
  - when LoRaWAN is not yet configured...
* LoRaWAN : investigate why sometimes downlinks are not working : tune Rx timing
  - run a testloop with linkCheckRequest, and have Rx1 start tuned to middle of still working values..
  - also think about how long to listen for rx rx1/rx2 timeout

* CLI : 
  - UART2 activeren bij USB connectie


* deviceStatusReq doen werken met echte waarden
  - ontvangstwaarden van laatste downlink opslaan
  - batterijspanning omzetten in uint8 : omrekening toevoegen aan battery


* batteryVoltage seems incorrect ?? also shown with 1 decimal io 2 ??
* wrapping of measurementsCollection : writing and reading measurements
* erase old measurements when space needed for new ones
* Give the node a name property which can be set from CLI / downlink, and which appears on the display status bar




# Medium Term ToDo's
* integrate qrCode as full static class. 
  - add unit testing
  - move code from screen to graphics





# Long Term ToDo's
* look at partial display updates : C:\Users\pasca\Documents\Projects\UA\GxEPD\examples\PartialUpdateExample
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
