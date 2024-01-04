#include "Display_EPD_W21_spi.h"
#include "main.h"

extern SPI_HandleTypeDef hspi2;




void SPI_Write(unsigned char value)                                    
{                                                           
//    unsigned char i;
//
//    for(i=0; i<8; i++)
//    {
//        EPD_W21_CLK_0;
//        if(value & 0x80)
//        	EPD_W21_MOSI_1;
//        else
//        	EPD_W21_MOSI_0;
//        value = (value << 1);
//        EPD_W21_CLK_1;
//    }
}

void EPD_W21_WriteCMD(unsigned char command)
{
	HAL_GPIO_WritePin(displayDataCommand_Port, displayDataCommand_Pin, GPIO_PIN_RESET); // Select Command
	HAL_GPIO_WritePin(displayChipSelect_Port, displayChipSelect_Pin, GPIO_PIN_RESET); // Drive CS low = SPI slave selection
	HAL_SPI_Transmit(&hspi2, &command, 1U, 20U);
	HAL_GPIO_WritePin(displayChipSelect_Port, displayChipSelect_Pin, GPIO_PIN_SET); // Drive CS low = end of SPI slave selection
}
void EPD_W21_WriteDATA(unsigned char data)
{
	HAL_GPIO_WritePin(displayDataCommand_Port, displayDataCommand_Pin, GPIO_PIN_SET); // Select Command
	HAL_GPIO_WritePin(displayChipSelect_Port, displayChipSelect_Pin, GPIO_PIN_RESET); // Drive CS low = SPI slave selection
	HAL_SPI_Transmit(&hspi2, &data, 1U, 20U);
	HAL_GPIO_WritePin(displayChipSelect_Port, displayChipSelect_Pin, GPIO_PIN_SET); // Drive CS low = end of SPI slave selection
}


