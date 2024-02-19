TX


      /* Disable the UART Transmit Data Register Empty Interrupt */
      ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_TXEIE_TXFNFIE);

      /* Enable the UART Transmit Complete Interrupt */
      ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_TCIE);
      /* Disable the UART Transmit Complete Interrupt */
        ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_TCIE);


      huart->Instance->TDR = (uint8_t)(*huart->pTxBuffPtr & (uint8_t)0xFF);



RX

    uhdata = (uint16_t) READ_REG(huart->Instance->RDR);
      /* Disable the UART Parity Error Interrupt and RXNE interrupts */
      ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE_RXFNEIE | USART_CR1_PEIE));

      /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
      ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);
