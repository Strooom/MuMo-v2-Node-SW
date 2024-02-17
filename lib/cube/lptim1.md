LPTIM1 is clocked by LSE @ 32768 Hz
LPTIM1 is a 16-bit timer -> 65536 range
With the prescaler 1, 2, 4, .., 128 this accomodates for the following range
  1 :   2 seconds,  30 uS resolution
  2 :   4 seconds,  61 uS resolution
  4 :   8 seconds, 122 uS resolution
  8 :  16 seconds, 244 uS resolution
 16 :  32 seconds, 488 uS resolution
 32 :  64 seconds, ~ 1 ms resolution
 64 : 128 seconds, ~ 2 ms resolution
128 : 256 seconds, ~ 4 ms resolution

As the Rx1 delay can be up to 15 seconds, let's select prescaler 8