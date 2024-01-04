# Partitions

1 : applicationSettings
    * startAddress = 0x0000
    * length = 4096 bytes

2 : measurementCollection
    * startAddress = 4096
    * length = 60K

Note : There is a second bank of 64K EEPROM, but currently is not yet used
Note : Whenever needed, it would be possible to also store measurements in the STM32WLE EEPROM, as we do have 256K and our application does not need all of that.