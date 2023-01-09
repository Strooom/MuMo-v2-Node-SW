# Feature List for Unit-Testing
1. Sensors
1.1 auto-detect sensors
1.2 read sensor data using oversampling
1.3 read sensor data using prescaling

2. Non Volatile Memory
2.1 Store sensor reading in NVM until reception by cloud application is confirmed
2.2 Store node configuration in NVM

3. Timing
3.1 Sync RTC with LoRaWAN time
3.2 keep offsets in NVM : leap-seconds / timezone



9. Other
9.1 When de firmware detects an empty EEPROM, it should run a production self-test routing to assist hardware manufacturing test