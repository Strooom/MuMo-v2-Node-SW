# TESTING

1. unit testing on desktop -> all generic unit tests
2. unit testing on target -> all target unit tests

# PROBLEMS

1. to build a functioning target unit test, a lot of extra code is required
    * unity_config.h / unity_config.cpp to set up the transport of test results back to the PC
    * I noticed that if unity_config.c is not renamed to .cpp, the linker has a problem (related to "#ifdef __cplusplus extern "C" #endif")
    * initialization of HAL, Clock and UART
    * this requires extra code into test.cpp, so it's not easy to make a test.cpp which runs on both desktop and target
    * testing on the target may also change the non-volatile storage, so they a test may have side-effects on other tests

2. ideas to make test run on both platforms :
    * put the files from 'src' into a (extra) lib folder instead
    * move HAL code generated by STM32CubeMx into a c++ wrapper, so it's easier to call on application startup
    * logging is still possible to the SWO debug port, logging to UART must be disabled in unit tests

3. Ideas
    * using TEST_MESSAGE you could give instructions to the test-person to check certain items : eg text shown on display
    * using TEST_MESSAGE you could give instructions to the test-person to take certain actions : eg "enable/disable USB power"
    * using a few hardware stubs, we could create loopbacks to test hardware features:
        - uart1 loopback
        - LEDs
        - I2C Loopback with eg MCP23008
    * testing that the epaper display is really in sleep mode 
    * testing the presence of sensors by having them on a separate PCB (one without an MCU would be ok)



# Coverage on Target
see article https://mcuoneclipse.com/2014/12/26/code-coverage-for-embedded-target-with-eclipse-gcc-and-gcov/


gcovr --html-details -o  ./test/coverage/main.html

https://gcovr.com/en/5.0/guide.html

https://pyocd.io/docs/semihosting.html