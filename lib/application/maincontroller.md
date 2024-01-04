* consists of a stateMachine.
    - RTC-tick starts the cycle : 
        * wake up the sensor
        * start sample
        * after all sensors are services, power them down..

sensorDeviceCollection : the list of sensors, eg bme680 sht40, ...
    has a discover method
    has a run method
sensorDeviceState : sleeping, ready, sampling