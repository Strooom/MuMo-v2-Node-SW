# Plan

1. get display working
    * detecting display from busy line
    * initialize and show imagefile
    * partial updates
    * 

2. get sensors working
3. store measurements into eeprom
4. get cli working
5. get LoRaWAN working


All subsystems need a function getState(). If they all return 'idle' or 'sleep', then the MCU can go sleep as well.
This will ensure
* display updates complete
* sensor readins complete
* eeprom writes complete
* 