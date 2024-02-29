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

 OK 1. write a target unit test which verifies the hardware AES for LoRaWAN encryptions
 1b. collect some real messages payloads from the old firmware, especially on the downlink direction
 - they can be used to test message processing and mac parsing etc.
 2. try a transmit cycle

 merge hw aes tests into 1, in order to not have too many target unit tests

 Put some of the aesKey stuff, only needed for SW-AES in conditional compilation, eg key expansion