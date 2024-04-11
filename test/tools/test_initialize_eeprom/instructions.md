This tool (implemented as a test) will initialize the contents of the EEPROM :
LoRaWAN state and channels




# How to use
* select the 'some_target_mumo_v2x_unittests' environment in PlatformIO
* in platformio.ini, in the section '[env:some_target_mumo_v2x_unittests]' set the test filter to
    test_filter = tools/test_initialize_eeprom
* configure the options you want in the source code :
    - set correct values for all settings
    - set the two bool flags to enforce overwriting existing LoRaWAN config and/or LoRaWAN state
* run the test :
    - either click the erlenmeyer logo a the bottom toolbar
    - or choose PlatformIO from the left sidebar, then 'Advanced | Test'
* all tests should report green/succes