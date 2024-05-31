# Plan / ToDo

* make unit tests for each sensor
* make unit tests for a sensorCollection which enables all sensors.
*. make EEPROM work for 128 K io 64K
*. take channel-initialization out of the device initialization and into sensorDeviceCollection
*. probleem bij het zoeken van measurementOffsets.. wanneer de nieuwe de oude data overschrijven, is mogelijk slechts een stuk ve measurement overschreven.. en kan uitlezen dus corrupt worden, dus bij overschrijven van metingen zou ineens de hele meting moeten overschreven worden..


1. get LowPower working again
2. make display show non-blocking
3. get cli working
    * detect commands
    * show version etc commands
    * set config commands
4. store measurements into eeprom
5. get LoRaWAN working

 

 # Small Next Steps

* extend stateMachine : if not LoRaWAN config found -> show msg asking for config
* extend SX126x code to the 14dBm veriant of the Wio-E5
* reading the tamper button and wake up from it
* 




* merge hw aes tests into 1, in order to not have too many target unit tests
* Put some of the aesKey stuff, only needed for SW-AES in conditional compilation, eg key expansion