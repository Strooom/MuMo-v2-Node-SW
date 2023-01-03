# Flexible schema to send data over LoRa

* using LoRaWAN implies using limited bandwidth, so sending only the minimum payload and with the minimum frequence
* different sensors may have different needs for bandwidth and sample-rate
* Cayenne takes care of this to a certain extent, but is still missing important info

So proposed solution is to define a number of channels, one for each property to be measured :
* batteryLevel
* temperature
* relativeHumidity
* barometricPressure
* lightIntensity
* ...

Assigning 1 byte to the 'channel' allows lots of different sensors, as well as improvements in the data-structure later on

Each type of sensor may benefit from an amount of 'oversampling', ie. take multiple samples which are then averaged into a single measurement value. The result is often a better accuracy as well as filtering of some measurement noise.

So each sensor will have a setting 'overSampling', (uint32_t)

Furhter, not all measurements change with the same frequency. Eg. temperature may rise quite quickly, eg 1 degreee / minute, but barometric pressure, battery-level etc are changing much slower. So they can be sampled less frequently.

Each sensor will have a 'preScaler' value (uint32_t), meaning that it is sampled every n sampling times, io always. 

Finally we need an overall sampling time setting, which controls the base frequency of taking samples, expressed in (eg) milliseconds. (uint32_t)
This allows to eg sample every 10 seconds, average 6 values so we send temperature every minute
Another example : set prescaler for batteryLevel to 360, so we only sample it every hour. Then set overSampling to 24, so we send battery level once every day.