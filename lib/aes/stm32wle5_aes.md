# Benchmarking HW AES vs SW AES

STM32WLE running @ 16 MHz
Application Payload : 128 bytes

                            SW              HW              Improvement
16K block encryptions :     9985 ms         291 ms          34x
2K message encryptions :    10629 ms        337 ms          32x
2K MIc calculations :       12982 ms        335 ms          39x

Memory footprint
ROM : SW = HW + 420 bytes
RAM : 2 * 176 bytes from expanded key

LinkedIn post

# LoRaWAN cryptography in Hardware

LoRaWAN messages are encrypted and secured, using AES-CTR for the application payload encryption and AES-CMAC for the Message Integrity Code.
Most LoRaWAN solutions do this in software, often in an inefficient 8-bit implementation.

That's not really smart, knowing that most LoRaWAN oriented MCU's (eg. the STM32WLE) have a hardware encrytion engine included.
My LoRaWAN library takes advantage of the HW encryption and here are the benchmarks, resulting in the following benefits:
* smaller ROM footprint (~420 bytes)
* smaller RAM footprint (~350 bytes)
* guaranteed standards compliant AES implementation
* payload encryption : 32x faster : 160 us io 5.2 ms
* MIC calculation : 39x faster : 160 us io 6.3 ms

STM32WLE running @ 16 MHz
Application Payload : 128 bytes

                            SW              HW              Improvement
16K block encryptions :     9985 ms         291 ms          34x
2K message encryptions :    10629 ms        337 ms          32x
2K MIc calculations :       12982 ms        335 ms          39x
