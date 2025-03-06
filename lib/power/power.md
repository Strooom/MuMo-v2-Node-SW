# What peripherals are required in each phase ?

1. Measurement          a few 100 ms    I2C
2. Logging into NVS     5 ms            I2C
3. Networking
3.1 Prepare Tx                          I2C
3.2 WaitForTx           0..8 sec LPTIM1
3.3 Tx
3.4 WaitForRx1          5 sec LPTIM1
3.5 Rx1
3.6 WaitForRx2          1 sec LPTIM1
3.7 Rx2
3.8 ProcessRxMessage                    I2C
4. Display                                      SPI
5. Sleep