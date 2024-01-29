
#ifndef ENCRYPT_H
#define ENCRYPT_H

/*
********************************************************************************************
* INCLUDES
********************************************************************************************
*/

#include "Struct.h"
#include <aeskey.hpp>
#include <aesblock.hpp>


/*
*****************************************************************************************
* FUNCTION PROTOTYPES
*****************************************************************************************
*/

void Construct_Data_MIC(sBuffer *Buffer, sLoRa_Session *Session_Data, sLoRa_Message *Message);
void Calculate_MIC(sBuffer *Buffer, unsigned char *Key, sLoRa_Message *Message);
void Encrypt_Payload(sBuffer *Buffer, unsigned char *Key, sLoRa_Message *Message);
void Generate_Keys(unsigned char *Key, unsigned char *K1, unsigned char *K2);
void Generate_Keys(aesKey &keyIn, aesKey &keyK1, aesKey &keyK2);
void Calculate_MIC2(sBuffer *Buffer, unsigned char *Key, unsigned char *New_Data);
uint32_t Calculate_MIC3(uint8_t *payload, uint32_t payloadLength, aesKey &keyIn);
void Shift_Left(unsigned char *Data);
void Shift_Left(aesBlock &blockInOut);

#endif
