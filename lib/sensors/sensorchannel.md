# sensor oversampling and output frequency

## oversampling

You can set any sensorChannel to take multiple samples and average them to a single output. This filters noise and improves the sensors accuracy.
Select oversampling from 4 possible values :
0 : no oversampling, each sample is an output 
1 : average 2 samples to an output
2 : average 4 samples to an output
3 : average 10 samples to an output

# output frequency

You can select an output frequency for any sensorChannel. Select from 14 possible values :
0 = off : channel will not produce outputs
1 = every minute
2 = every 2 minutes
3 = every 5 minutes
4 = every 10 minutes
5 = every 15 minutes
6 = every 30 minutes
7 = every hour
8 = every 2 hours
9 = every 4 hours
10 = every 6 hours
11 = every 12 hours
12 = every 24 hours
13 = every 48 hours

# combining oversampling and output frequency

The basic realTime clock tick is 30 seconds. This means that the sensor cannot be sampled more than once per 30 seconds.
As a result some combinations of oversampling and output frequency are not valid, eg
- oversampling set to 10
- output frequency set to every minute
This would require 10 samples per minute, and the maximum is 2 samples per minute
When setting incompatible values, the oversampling will be reduced to be compatible with output frequency

# optimizing oversampling and output frequency

For optimal (lowest) power consumption, it is best to consider 2 things :
* give all sensors settings so that the samples and outputs are multiples of each other (or equal). 
  * good example : one sensor every 60 minutes, other sensor 15 minutes (60 is multiple of 15)
  * bad example : noe sensor every 5 minutes, other sensor every 2 minutes (5 is not a multiple of 2)
* even when these settings are optimized, it is recommended to restart the device (SWR) so all counters are synchronized 

