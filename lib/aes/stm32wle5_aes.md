# Benchmarking HW AES vs SW AES

STM32WLE running @ 16 MHz
Application Payload : 128 bytes

                            SW              HW              Improvement
16K block encryptions :     9985 ms         291 ms          34x
2K message encryptions :    10629 ms        337 ms          32x
2K MIc calculations :       12982 ms        335 ms          39x