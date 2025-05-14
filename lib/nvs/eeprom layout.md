# Non-Volatile Memory
Implemented with one or more I2C eeprom chips.
Different HW-types can be used, if they are compatible with the following common addressing : 
* each bank of 64K bytes is addressed by incrementing the I2C address, from baseAddress = 0x50
* inside each bank a 16 bit address is used.

# Paging
Every memory access has some overhead : 
* sending the address in I2C
* disabling / re-enabling the write-protect for write operations
So reading or writing a range of bytes is more efficient.
However, this can only be done when the range is inside a so-called 'page' of the EEPROM device, typically 128 or 256 bytes.
So when doing reads or writes that span multiple EEPROM pages, they have to be split into separate I2C communications, with each communication reading/writing from a single page.

# Partitions

1 : applicationSettings
    * startAddress = 0x0000
    * length = 4096 bytes

2 : measurementCollection
    * startAddress = 4096
    * length = 60K

Note : There is a second bank of 64K EEPROM, but currently is not yet used
Note : Whenever needed, it would be possible to also store measurements in the STM32WLE EEPROM, as we do have 256K and our application does not need all of that.