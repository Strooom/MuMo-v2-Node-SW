# prescaling and oversampling

1. Prescaling means that a sensorChannel does not take a sample (measurement) every RTC tick (30s), but rather once per n RTC ticks.
The prescaler value can be set from 0 .. 4095
    prescaler = 0 : this sensorChannel does NOT take any samples = deactivation of the sensorchannel
    prescaler = 1 .. 4095 : take a sample every 1..4095 ticks. The prescaleCounter runs from (prescaler-1) to 0

2. Oversampling means that a sensorChannel takes multiple samples, and then averages them into a single output
The oversampling can be set from 0..15, resulting in averaging 1..16 samples to a single output.
The oversamplingCounter runs from (oversampling) to 0


Note : (minutesBetweenOutput * 2) must be a multiple of numberOfSamplesToAverage
Note : ((minutesBetweenOutput * 2) % numberOfSamplesToAverage) == 0


# TODO

* calibratie coefficienten in array steken
* code refactoren voor meer performantie : meer proberen op voorhand te berekenen
* store the oversampling / prescaling etc for the measurement channels in NVS so the settings remain after reset

# TODO
* the SX126x also needs two sensorChannels, for RSSI and SNR, as we will measure those and log, store, send them

We need an additional setting on each sensorChannel what to do with the measurement :
* nothing
* log to UART
* store into EEPROM
* transmit over LoRaWAN

Showing on the display is controlled at the display itself, where we define up to three sensorChannelTypes to be shown on the top 3 lines of the screen. Battery SOC and network is always shown at bottom line

# TODO
At startup, as not all samples in the array are written, the averaging won't work. Could solve this by doing a first sample and set all values to this value
Warning : Lux sensor does something wrong on first sample after power up