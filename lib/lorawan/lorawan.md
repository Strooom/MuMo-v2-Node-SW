# LoRaWAN Networking

The LoRaWAN Networking Driver is composed of 4 sections
1. Construction / Parsing of LoRa message
2. Encryption, Decryption & MIC calculation
3. TxRxCycle stateMachine
4. MAC Commands parsing and processing


## 1. Construction / Parsing of LoRa message

For Tx, the LoRaWAN message is constructed in the **rawMessage** buffer.
This buffer is long enough to hold:
* the B0 block, needed for MIC calculation - 16 bytes
* the LoRa payload - 256 bytes
* some padding - 15 bytes : the AES used for payload encryption/decryption and the MIC calculation works on blocks of 16 bytes. When the input to these functions is no exact multiple of 16 bytes, the buffer is padded with some additional padding bytes to make it a multiple of 16 bytes.

Two functions calculate all offsets and lengths of fields inside this rawMessage in order to make insertion or extraction of them easy
* **setOffsetsAndLengthsTx(framePayloadLength, frameOptionsLength)**
* **setOffsetsAndLengthsRx(loRaPayloadLength)**

To facilitate encryption, decryption and MIC calculation, the buffer has the B0 block at the front, and provides padding at the back, in case the input of the encryption would be less than a multiple of 16 bytes (128-bits).


## 2. Encryption, Decryption & MIC calculation

LoraWAN encryption is AES-128, which works on 'blocks' of 128-bit data (aesBlock) using 128-bit keys (aesKey)
* **networkKey** (MAC commands/answers encryption and MIC calculation)
* **applicationKey** (application payload encryption)

On the STM32WLE there is an AES-accelerator which performs all needed cryptographic operations in hardware:
* encrypting an aesBlock (in generateKeysK1K2()). Using **AES-EBC**
* encrypting / decrypting the **application payload**. Using **AES-CTR** mode
    - the A1 block is calculated and loaded into the AES initializationVector. Subsequent An blocks are calculated inside the AES engine
    - if needed, the to-be-encrypted data is padded with zeroes
    - the to-be-encrypted data is written to the AES engine and then encrypted contents is read back and written to the rawMessage. This realizes 'in-place' encryption. The algorythm is the same for encryption and decryption, as both are an XOR of input with the An-encrypted blocks.
* calculating the **MIC**. Using the **AES-CBC** mode
    - the B0 block is calculated
    - if needed, the to-be-encrypted data is padded (with 0x80 and zeroes)
    - the AES initializationVector is set to zeroes
    - the to-be-encrypted data is written to the AES engine.
    - the last block is XORed with K1 or K2, then written to the AES-engine.
    - the ouput of the engine then yields the MIC (output truncated to first 4 bytes).


## 3. TxRxCycle stateMachine

The LoRaWAN Class A cycle consists of a transmission (Tx) followed by listening for an answer in 1 or 2 receive time-windows, called Rx1 and Rx2.
The whole sequence and the required timings are managed by a stateMachine. A sequence consists of
* waitForRandomTimeBeforeTransmit
* waitForTxComplete
* waitForRx1Start
* waitForRx1CompleteOrTimeout
* waitForRx2Start
* waitForRx2CompleteOrTimeout
* waitForRxMessageReadout
* idle/sleep

As the waiting-times in this sequence are typically several seconds, the MCU will be put to sleep in order to reduce power consumption. To achieve accurate timing the LPTIM1 timer peripheral is used, which can wake up the MCU from sleep.


## 4. MAC Commands parsing and processing

In order to process MAC commands, there are 2 buffers:
* linearBuffer<macInOutLength> **macIn**
* linearBuffer<macInOutLength> **macOut**

If the application wants to send certain MAC commands, or needs to respond to received MAC commands, they are added to **macOut**.
When a received LoRaWAN message contains MAC commands, they are stored in **macIn**, and processed afterwards.

Some MAC commands are 'sticky', ie. they need to be repeated on every transmission until confirmed by a received message.


# Could Do
// TODO : if we have sticky MACstuff, we could append a linkcheckrequest and thus force the LNS to send us a downlink, which confirms the sticky MAC stuff...
processLinkAdaptiveDataRateRequest    // TODO : currently I don't understand the purpose of this mac command, so I just ignore it. Maybe it's more useful in US-915 than EU-868
processDutyCycleRequest    // TODO : until we implement dutyCycle management, we ignore this command
        processLinkCheckAnswer// TODO : For the time being, we just show this info in the logging. Later we could use it to show the quality of the link on the display