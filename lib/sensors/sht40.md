The SHT40 has no mechanism to check if data is ready.
So best is to do as follows:
* start sampling by sending command getMeasurementHighPrecision and log sysTick value
* then sample should be ready about 10 ms later (8.3 ms datasheet)
* then issue the I2C readcommand.


SHT40 has no sleep mode, no need to wake up.
Requires about 1 ms to power up