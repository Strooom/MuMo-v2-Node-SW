# LoRaWAN layer TODO's

1. store the complete non-volatile context in NVS and retrieve it from there upon initialization
  * currentDataRateIndex
  * all channel info : frequencies and datarates
  * rx1Delay

  are already stored : 
  * devAdrr
  * uplinkFrameCount
  * downlinkFrameCount
  * networkKey
  * ApplicationKey


2. Add an accurate transmit timeout to the SX126 - depends on loraPayload and dataRate
3. add an accurate receive timeout to the SX126 - depends on dataRate
4. Sx126b datasheet says we should check the CRC of the message. Is this needed ? I think in downlink, no CRC is used..


5. semtech code checks if the number of bytes in macstuff is enough for a valid command...

1. implement more details of the execution of the MAC commands and answers
2. reconsider frameOptions upstream, because they are required for acknowledging certain commands..
3. when using frameOptions upstream, the amount of available payload in framePayload changes..
4. channels seem to have an INDEX, so I cannot assume a contiguous list of channels in channelCollection, and we need an 'enable' for each individual channel. This has an impact on selecting a random channel
5. provide channelSelection over ALL active channels + over the 3 DEFAULT channels..(so we can send MAC stuff over those 3 only)
6. implementation of dutyCycle
7. monitor the timing of the txRxCycle on the scope and optimize the timings - they are very coarse for the time being


# LoRaWAN unitTests

* unitTest for each MAC command
  - set context
  - execute request / answer
  - check context

* test different signatures for insertPayload

adding lorawan channels : use index io frequency.. when freq = 0, channel is inactive


# LoRaWAN compliance
* port 224
* cycleTime : 5 seconds


void SUBGRF_GetPacketStatus( PacketStatus_t *pktStatus )
{
    uint8_t status[3];

    SUBGRF_ReadCommand( RADIO_GET_PACKETSTATUS, status, 3 );

    pktStatus->packetType = SUBGRF_GetPacketType( );
    switch( pktStatus->packetType )
    {
        case PACKET_TYPE_GFSK:
            pktStatus->Params.Gfsk.RxStatus = status[0];
            pktStatus->Params.Gfsk.RssiSync = -status[1] >> 1;
            pktStatus->Params.Gfsk.RssiAvg = -status[2] >> 1;
            pktStatus->Params.Gfsk.FreqError = 0;
            break;

        case PACKET_TYPE_LORA:
            pktStatus->Params.LoRa.RssiPkt = -status[0] >> 1;
            // Returns SNR value [dB] rounded to the nearest integer value
            pktStatus->Params.LoRa.SnrPkt = ( ( ( int8_t )status[1] ) + 2 ) >> 2;
            pktStatus->Params.LoRa.SignalRssiPkt = -status[2] >> 1;
            pktStatus->Params.LoRa.FreqError = FrequencyError;
            break;

        default:
        case PACKET_TYPE_NONE:
            // In that specific case, we set everything in the pktStatus to zeros
            // and reset the packet type accordingly
            RADIO_MEMSET8( pktStatus, 0, sizeof( PacketStatus_t ) );
            pktStatus->packetType = PACKET_TYPE_NONE;
            break;
    }
}