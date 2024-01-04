# prescaling and oversampling

1. Prescaling means that a sensorChannel does not take a sample (measurement) every RTC tick (30s), but rather once per n RTC ticks.
The prescaler value can be set from 0 .. 4095
    prescaler = 0 : this sensorChannel does NOT take any samples = deactivation of the sensorchannel
    prescaler = 1 .. 4095 : take a sample every 1..4095 ticks. The prescaleCounter runs from (prescaler-1) to 0

2. Oversampling means that a sensorChannel takes multiple samples, and then averages them into a single output
The oversampling can be set from 0..15, resulting in averaging 1..16 samples to a single output.
The oversamplingCounter runs from (oversampling) to 0