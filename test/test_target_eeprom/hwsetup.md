In order to run this target unit tests, you need to take following HW setup :
* testPort = UART1 = ST-Link, normally COM14

Important Remark : 
This test tries to test the EEPROM without changing it's contents :
* the current contents of the EEPROM is read
* then a new value is written, read back and verified.
* afterwards, the original contents is written, read back and verified.
If all tests pass, the contents of the EEPROM should not have changed. If some tests did NOT work, then the contents could have changed.