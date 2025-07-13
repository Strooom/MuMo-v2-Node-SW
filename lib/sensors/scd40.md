Important note : I found the temperature readings of the sensor to be very inaccurate : 4 deg C lower than BME680. 
It seems this sensor is not factory calibrated and need manual calibration via its offset register. 
As we are using the sensor in low power mode, it's maybe dissipating less heat and as such giving a too low reading.