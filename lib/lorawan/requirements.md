# LoRaWAN v1.0.4 Requirements derived from LoRaWAN® L2 1.0.4 Specification (TS001-1.0.4)

This is a list of requirements distilled from the Link Layer Specification document.
Each requirement implemented in the code, can be found in the comments using its index

## 1. Introduction
* 1.001 [292] The end-device changes channels in a pseudo-random fashion for every transmission. The resulting frequency diversity makes the system more robust to interference. The end-device pseudo-randomly changes its transmit periodicity to prevent systematic synchronization of populations of end-device transmissions. 
* 1.002 [299] All LoRaWAN end-devices SHALL implement at least Class A functionality.
* 1.003 [323] The octet order for all multi-octet fields SHALL be little endian.
* 1.004 [324] By default, RFU bits are Reserved for Future Use and SHALL be set to 0 by the transmitter of the frame and SHALL be silently ignored by the receiver

## 2. Introduction to LoRaWAN Options

## 3. Physical Packet Formats
* 3.001 [376] Following each uplink transmission, the end-device SHALL open one or two receive windows (RX1 and RX2); if no packet destined for the end-device is received in RX1, it SHALL open RX2. The receive windows start times are defined using the end of the transmission as a reference
* 3.002 [385] If a preamble is detected during one of the receive windows, the radio receiver SHOULD stay active until the downlink frame is demodulated.
* 3.003 [386] If a frame was detected and subsequently demodulated during the first receive window, and the frame was intended for this end-device after address and MIC (message integrity code) checks, the end-device SHALL NOT open the second receive window.
* 3.004 [391] The first receive window RX1 uses a frequency that is a function of the uplink frequency and a data rate that is a function of the uplink data rate.
* 3.005 [392] RX1 SHALL be open no longer than RECEIVE_DELAY1 seconds after the end of the uplink modulation
* 3.006 [397] The second receive window RX2, if opened (see Section 3.3.1), uses a fixed configurable frequency and data rate
* 3.007 [398] The second receive window RX2 SHALL be open no longer than RECEIVE_DELAY2 seconds after the end of the uplink modulation
* 3.008 [403] The duration of a receive window SHALL be at least the time required by the end-device’s radio transceiver to detect a downlink preamble
* 3.009 [407] The end-device receive window duration has to accommodate the maximum potential imprecision of the end-device’s clock
* 3.010 [425] An end-device SHALL NOT transmit another uplink packet before it has either received a downlink packet in the first or second receive window related to the previous transmission or if the second receive window related to the previous transmission has expired


## 4. MAC Frame Formats

