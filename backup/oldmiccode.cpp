// uint32_t LoRaWAN::calculateMic(uint8_t* payload, uint32_t payloadLength) {
//     aesBlock outputBlock;
//     uint8_t outputAsBytes[16];

//     unsigned char byteIndex, blockIndex;
//     unsigned char Old_Data[16]       = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//     uint32_t nmbrOfBlocks            = aesBlock::nmbrOfBlocks(payloadLength);
//     uint32_t incompleteLastBlockSize = aesBlock::incompleteLastBlockSize(payloadLength);

//     // Perform full calculating until n-1 message blocks
//     for (blockIndex = 0x0; blockIndex < (nmbrOfBlocks - 1); blockIndex++) {
//         outputBlock.setFromByteArray(payload + (blockIndex * 16));        //  Copy data into block

//         // for (i = 0; i < 16; i++) {
//         //     outputAsBytes[i] = Buffer->Data[(j * 16) + i];
//         // }

//         // Perform XOR with old data
//         // outputBlock.set(outputAsBytes);
//         outputBlock.XOR(Old_Data);        // XOR(outputAsBytes, Old_Data);
//         memcpy(outputAsBytes, outputBlock.asBytes(), 16);

//         // Perform AES encryption
//         outputBlock.setFromByteArray(outputAsBytes);
//         outputBlock.encrypt(networkKey);        // AES_Encrypt(outputAsBytes, Key);
//         memcpy(outputAsBytes, outputBlock.asBytes(), 16);

//         // Copy New_Data to Old_Data
//         for (byteIndex = 0; byteIndex < 16; byteIndex++) {
//             Old_Data[byteIndex] = outputAsBytes[byteIndex];
//         }
//     }

//     // Perform calculation on last block
//     // Check if Datalength is a multiple of 16
//     if (incompleteLastBlockSize == 0) {
//         outputBlock.setFromByteArray(payload + ((nmbrOfBlocks - 1) * 16));        //  Copy data into block

//         // Copy last data into array
//         // for (i = 0; i < 16; i++) {
//         //     outputAsBytes[i] = Buffer->Data[((nmbrOfBlocks - 1) * 16) + i];
//         // }

//         // Perform XOR with Key 1
//         // outputBlock.set(outputAsBytes);
//         outputBlock.XOR(keyK1.asBytes());        //  XOR(outputAsBytes, Key_K1);
//         // memcpy(outputAsBytes, outputBlock.asBytes(), 16);

//         // Perform XOR with old data
//         // outputBlock.set(outputAsBytes);
//         outputBlock.XOR(Old_Data);        // XOR(outputAsBytes, Old_Data);
//         // memcpy(outputAsBytes, outputBlock.asBytes(), 16);

//         // Perform last AES routine
//         // outputBlock.set(outputAsBytes);
//         outputBlock.encrypt(networkKey);        // AES_Encrypt(outputAsBytes, Key);
//         // memcpy(outputAsBytes, outputBlock.asBytes(), 16);

//     } else {
//         // Copy the remaining data and fill the rest
//         for (byteIndex = 0; byteIndex < 16; byteIndex++) {
//             if (byteIndex < incompleteLastBlockSize) {
//                 outputAsBytes[byteIndex] = payload[((nmbrOfBlocks - 1) * 16) + byteIndex];
//             }
//             if (byteIndex == incompleteLastBlockSize) {
//                 outputAsBytes[byteIndex] = 0x80;
//             }
//             if (byteIndex > incompleteLastBlockSize) {
//                 outputAsBytes[byteIndex] = 0x00;
//             }
//         }

//         // Perform XOR with Key 2
//         outputBlock.setFromByteArray(outputAsBytes);
//         outputBlock.XOR(keyK2.asBytes());        //  XOR(outputAsBytes, Key_K2);
//         // memcpy(outputAsBytes, outputBlock.asBytes(), 16);

//         // Perform XOR with old data
//         // outputBlock.set(outputAsBytes);
//         outputBlock.XOR(Old_Data);        // XOR(outputAsBytes, Old_Data);
//         // memcpy(outputAsBytes, outputBlock.asBytes(), 16);

//         // Perform last AES routine
//         // outputBlock.set(outputAsBytes);
//         outputBlock.encrypt(networkKey);        // AES_Encrypt(outputAsBytes, Key);
//         // memcpy(outputAsBytes, outputBlock.asBytes(), 16);
//     }
//     uint32_t mic = ((outputBlock.asBytes()[0] << 24) + (outputBlock.asBytes()[1] << 16) + (outputBlock.asBytes()[2] << 8) + (outputBlock.asBytes()[3]));
//     return mic;
// }

