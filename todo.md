# ToDo - code
* read VBAT through ADC
* read onboard sensor through I2C : LM75ADP
* read TSL25911 and/or BME680 through I2C


# ToDo - devops
* see if we can program via Black Magic Probe
* see if we can debug via Black Magic Probe
* maybe buy another ST-Link V2 to hack and enable debugging

# Logbook
2023/01/05 : got I2C working, at least an I2C scan on the Wio-E5 development board
2023/01/05 : added transmitting a '.' through serial for every toggle of the LED
2023/01/05 : built a simple Blink application, similar as in ST tutorials. Using LED on PB5

# IDEAS
1. if the firmware detects an empty eeprom, it should perform PCB-HW self test.



# Remember
1. monitoring VBAT : when this ADC channel is enabled, it activates a 1:3 resistor divider which consumes some current. So for minimal power, this channel should be enabled only during the measurement.

2. I2C scanner : on the Wio-E5 Development board, the 3.3V needs to be enabled in software by setting PA9 high. I2C will not work without this, as the 3.3V is needed for the pullup of the SCL SDA lines. Seeed has done an interesting design though, in enabling some circuitry through a transistor. Could be interesting for V2.1 to get to even lower power.