https://lorawan-packet-decoder-0ta6puiniaut.runkit.sh/


# LoRaWAN layer TODO's

## Short Term

1. Store changes in state / channels into NVS after all macIn handling is done.
 - simple : save state and channels after all MAC commands are processed... -> we will often save when nothing changed
 - more advanced : keep track of changes in bool flag(s) and only save if flag is set during MAC processing
2. Dump RF measurements after packet reception
3. Add an accurate transmit timeout to the SX126 - depends on loraPayload and dataRate : this we can calculate, and as timeout usually does not occur, a bit oversizing is no problem
4. add an accurate receive timeout to the SX126 - depends on dataRate : this we can maybe determine from trial and error : it also accomodates clock and timer accuracies.. oversizing is wasting energy, as 
5. Get RF signal from SX126x reception
6. Rx1DataRate should use rx1DataRateOffset
7. rx2DataRateIndex should be initialized to SF9, but could change




## Medium Term
1. implement ADR
2. implementation of dutyCycle



 

# MAC-Layer
1. When activating new channels, sending the answer over a new channel : does this work for TTI ?
2. LinkCheckRequest
3. TimingRequest


# Payload Formatting
1. io sending payloadEncodingVersion as a payload byte, use a port for that...
2. MSB of dataChannel controls if it has a timestamp
3. B6..B2 : determine sensorDevice -> max 32 types of devices
4. B1..B0 : sensorDeviceChannel -> max 4 channels per device






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


