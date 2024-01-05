# How to configure UART in STM32

* set dataWidth to 8 bits : UASRT_CR1 M0 bits = 0b00
* enable FIFO (if possible only on Tx, not on Rx) USART_CR1 FIFOEN bit
* set thresholds for FIFO : USART_CR3 RXFTCFG TXFTCFG
* start transmission : set TE (TransmitEnable) flag
* note : set TE first, then write data to the FIFO.. ??
* stop bits : UASRT_CR3 : 1 stop bit is default..


To transmit a character, follow the sequence below:
1. Program the M bits in USART_CR1 to define the word length.
2. Select the desired baud rate using the USART_BRR register.
3. Program the number of stop bits in USART_CR2.
4. Enable the USART by writing the UE bit in USART_CR1 register to 1.
5. Select DMA enable (DMAT) in USART_CR3 if multibuffer communication must take 
place. Configure the DMA register as explained in Section 33.5.19: Continuous 
communication using USART and DMA.
6. Set the TE bit in USART_CR1 to send an idle frame as first transmission.
7. Write the data to send in the USART_TDR register. Repeat this for each data to be 
transmitted in case of single buffer. 
– When FIFO mode is disabled, writing a data to the USART_TDR clears the TXE 
flag. 
– When FIFO mode is enabled, writing a data to the USART_TDR adds one data to 
the TXFIFO. Write operations to the USART_TDR are performed when TXFNF 
flag is set. This flag remains set until the TXFIFO is full.
8. When the last data is written to the USART_TDR register, wait until TC = 1. 
– When FIFO mode is disabled, this indicates that the transmission of the last frame 
is complete.
– When FIFO mode is enabled, this indicates that both TXFIFO and shift register are 
empty.
This check is required to avoid corrupting the last transmission when the USART is 
disabled or enters Halt mode


To receive a character, follow the sequence below:
1. Program the M bits in USART_CR1 to define the word length.
2. Select the desired baud rate using the baud rate register USART_BRR 
3. Program the number of stop bits in USART_CR2.
4. Enable the USART by writing the UE bit in USART_CR1 register to ‘1’.
5. Select DMA enable (DMAR) in USART_CR3 if multibuffer communication is to take 
place. Configure the DMA register as explained in Section 33.5.19: Continuous 
communication using USART and DMA. 
6. Set the RE bit USART_CR1. This enables the receiver which begins searching for a 
start bit