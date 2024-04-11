# Plan / ToDo

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