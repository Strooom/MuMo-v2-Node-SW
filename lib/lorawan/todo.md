# LoRaWAN layer TODO's

io sending payloadEncodingVersion as a payload byte, we could use a port for that...

* need a solution for units without config, state and channels in NVS -> restore doesn't restore proper data

* need a solution for units on V0 firmware, with only devAdrr, networkKey and applicationKey in NVS : how to initialize the rest of the NVS
 - a 1-time firmware who does the migration <-- preferred : it can also dump the results to logging..
 

2. Add an accurate transmit timeout to the SX126 - depends on loraPayload and dataRate
3. add an accurate receive timeout to the SX126 - depends on dataRate
5. semtech code checks if the number of bytes in macstuff is enough for a valid command...
5. provide channelSelection over ALL active channels + over the 3 DEFAULT channels..(so we can send MAC stuff over those 3 only)
6. implementation of dutyCycle
7. monitor the timing of the txRxCycle on the scope and optimize the timings - they are very coarse for the time being
8. implement ADR
9. Get RF signal from SX126x reception

# TxRxCycle 
* get the timings right, now that lptim1 is running @ 4.096 kHz
* see if we can sleep the MCU when waiting
* Try everything in a target unit test

# LoRaWAN compliance
* port 224
* cycleTime : 5 seconds


To get RF quality index : 
1. send linkcheckrequest
2. after linkcheckanswer : uplink quality is in mac answer
3. downlink quality is available from sx126 as shown below.

void SUBGRF_GetPacketStatus( PacketStatus_t *pktStatus )
{
    uint8_t status[3];
    SUBGRF_ReadCommand( RADIO_GET_PACKETSTATUS, status, 3 );

    pktStatus->Params.LoRa.RssiPkt = -status[0] >> 1;
    // Returns SNR value [dB] rounded to the nearest integer value
    pktStatus->Params.LoRa.SnrPkt = ( ( ( int8_t )status[1] ) + 2 ) >> 2;
    pktStatus->Params.LoRa.SignalRssiPkt = -status[2] >> 1;

    pktStatus->Params.LoRa.FreqError = FrequencyError;

}


