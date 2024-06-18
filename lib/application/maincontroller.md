# Application

At the top-level, the application consists of a **state-machine**, implemented in **mainController**.
This ensures that 
* all actions occur in the right sequence : see below
* prevents that asynchronous events can corrupt the application state
    - eg. no changing of LoRaWAN parameters during the txRxCycle phase
    - 
* the MCU is put in correct state before going into sleep
    - MCU goes into sleep when the application sequence is completed and is back in **idle** state
    - MCU goes into sleep while waiting for timeouts during **LoRaWAN txRxCyle**

## Main Application Sequence Cycle

1. wake up on RTC tick (every 30 seconds)
2. run the sensorDeviceCollection : this triggers sensors to do a measurement.
3. store the sensor-measurements in EEPROM
4. run the LoRaWAN txRxCycle
5. store the network-measurements in EEPROM
6. log the measurements to log output
7. update the display
8. wait idle or into sleep until next RTC-tick

## CLI

When the device is connected by USB to a host computer, it receives 5V power.
When receiving power:
* the MCU will not go into sleep
* the UART1 is enabled
* in idle state, the MCU processes CLI commands


## StateMachine top-level
* boot
* waitForBootScreen : shows data on e-paper display (skipped if no display present)
  - copyright
  - buildinfo version - buildtype
  - buildtimestamp
  - sensors found
  - 
* waitForNetworkResponse : send some mac commands, expect an answer. Retry with higher SF if no answer.. Give up if no answer at all..
  - used SF
  - SNR
  - actual time
    
* then idle and run a cycle every 30 seconds
