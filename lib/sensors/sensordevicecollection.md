sensorDeviceCollection contains a list of sensorDevices
each sensorDevice contains 1 or more channels (hardcoded number for each devicetype)
each channel has a number of properties
* name
* units
* decimals
* oversampling
* prescaling

a sensorDevice is active/inactive as controlled by sensorDeviceCollection::isPresent[]
a channel is active/inactive as controller by prescaler :
    prescaler = 0 : inactive
    prescaler > 0 : active

Typical sequence for running all sensors is :
* sensorDeviceCollection::needsSampling() : check if sampling is needed on any of the devices / channels
* if sampling is needed : startSampling()
* then poll for samplingIsReady()
* sensorDeviceCollection::hasNewMeasurements() : then check for new measurements 
* if any, collect all new measurement in a measurementsGroup
* update channel counters, to set them ready for next tick
