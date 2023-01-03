This repository will hold the firmware for the MuMo node, a LoRa-based environmental monitor for museum collection items.
The hardware can be found here : https://github.com/Strooom/MuMo-V2-Node-PCB

Currently we are working on the hardware. The firmware will be developed in 2023Q1

# Firmware Development Plan : TODO's
1. explore the existing demo appliction from ST / Seeed
2. create template project in STM32CubeIDE with correct settings for all HW
3. run a Blinky on the target HW
4. find a solution for unitTesting
5. add libraries for sensors BME680, TSL25911
6. add library for ePaper display and eeprom
7. investigate low power behaviour - sleep modes
8. implement uplink to TheThingsStack / TheThingsNetwork
9. implement downling from TheThingsStack / TheThingsNetwork