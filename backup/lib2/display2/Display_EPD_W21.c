#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "main.h"

extern SPI_HandleTypeDef hspi2;

//void delay_xms(unsigned int xms)
//{
//	unsigned int i;
//	while(xms--)
//	{
//		i=12000;
//		while(i--);
//	}
//}

void EPD_Display(unsigned char *Image) {
	unsigned int Width, Height, i, j;
	Width = (EPD_WIDTH % 8 == 0) ? (EPD_WIDTH / 8) : (EPD_WIDTH / 8 + 1);
	Height = EPD_HEIGHT;

	EPD_W21_WriteCMD(0x24);
	for (j = 0; j < Height; j++) {
		for (i = 0; i < Width; i++) {
			EPD_W21_WriteDATA(Image[i + j * Width]);
		}
	}
	EPD_Update();
}

//void Epaper_Spi_WriteByte(unsigned char TxData)
//{
//	unsigned char TempData;
//	unsigned char scnt;
//	TempData=TxData;
//
//  EPD_W21_CLK_0;
//	for(scnt=0;scnt<8;scnt++)
//	{
//		if(TempData&0x80)
//		  EPD_W21_MOSI_1 ;
//		else
//		  EPD_W21_MOSI_0 ;
//		EPD_W21_CLK_1;
//	  EPD_W21_CLK_0;
//		TempData=TempData<<1;
//
//  }
//
//}

void Epaper_READBUSY(void) {
	GPIO_PinState busyPin;
	while (1) {
		busyPin = HAL_GPIO_ReadPin(displayBusy_Port, displayBusy_Pin);
		if (busyPin == GPIO_PIN_RESET) {
			return;
		};
	}
}

void Epaper_Write_Command(unsigned char cmd) {
	HAL_GPIO_WritePin(displayDataCommand_Port, displayDataCommand_Pin,
			GPIO_PIN_RESET); // Select Command
	HAL_GPIO_WritePin(displayChipSelect_Port, displayChipSelect_Pin,
			GPIO_PIN_RESET); // Drive CS low = SPI slave selection
	HAL_SPI_Transmit(&hspi2, &cmd, 1U, 20U);
	HAL_GPIO_WritePin(displayChipSelect_Port, displayChipSelect_Pin,
			GPIO_PIN_SET); // Drive CS low = end of SPI slave selection
}

void Epaper_Write_Data(unsigned char data) {
	HAL_GPIO_WritePin(displayDataCommand_Port, displayDataCommand_Pin,
			GPIO_PIN_SET); // Select Data
	HAL_GPIO_WritePin(displayChipSelect_Port, displayChipSelect_Pin,
			GPIO_PIN_RESET); // Drive CS low = SPI slave selection
	HAL_SPI_Transmit(&hspi2, &data, 1U, 20U);
	HAL_GPIO_WritePin(displayChipSelect_Port, displayChipSelect_Pin,
			GPIO_PIN_SET); // Drive CS low = end of SPI slave selection
}

void EPD_RESET(void) {
	HAL_GPIO_WritePin(displayReset_Port, displayReset_Pin, GPIO_PIN_RESET); // Drive RESET low
	HAL_Delay(20);
	HAL_GPIO_WritePin(displayReset_Port, displayReset_Pin, GPIO_PIN_SET); // Let RESET high
	HAL_Delay(20);

}

//SSD1681
void EPD_HW_Init(void) {
	EPD_RESET();

	Epaper_READBUSY();
	Epaper_Write_Command(0x12);  //SWRESET
	HAL_Delay(10);
	Epaper_READBUSY();

	Epaper_Write_Command(0x01); //Driver output control      
	Epaper_Write_Data(0xC7);
	Epaper_Write_Data(0x00);
	Epaper_Write_Data(0x00);

	Epaper_Write_Command(0x11); //data entry mode        X+ Y+
	Epaper_Write_Data(0x11);

	Epaper_Write_Command(0x44); //set Ram-X address start/end position : 200 pixels / 8 = 25 bytes : [0..24]
	Epaper_Write_Data(0x00); // x bytes start at 0
	Epaper_Write_Data(0x18); // x bytes end at 24 (0x18)

	Epaper_Write_Command(0x45); //set Ram-Y address start/end position          
	Epaper_Write_Data(0xC7);    // y rows run from 0 - 199 (0xC7)
	Epaper_Write_Data(0x00);
	Epaper_Write_Data(0x00);
	Epaper_Write_Data(0x00);

	Epaper_Write_Command(0x18); //Use built-in temperature sensor [0x80] / external sensor [0x48]
	Epaper_Write_Data(0x80);

	Epaper_Write_Command(0x3C); //BorderWavefrom
	Epaper_Write_Data(0x05);

	Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
	Epaper_Write_Data(0x00);
	Epaper_Write_Command(0x4F);   // set RAM y address count to 0X199;    
	Epaper_Write_Data(0xC7);
	Epaper_Write_Data(0x00);
	Epaper_READBUSY();

}
void EPD_HW_Init_GUI(void) {
	EPD_RESET();

	Epaper_READBUSY();
	Epaper_Write_Command(0x12);  //SWRESET
	Epaper_READBUSY();

	Epaper_Write_Command(0x01); //Driver output control      
	Epaper_Write_Data(0xC7);
	Epaper_Write_Data(0x00);
	Epaper_Write_Data(0x01); //Show mirror

	Epaper_Write_Command(0x11); //data entry mode       
	Epaper_Write_Data(0x11);

	Epaper_Write_Command(0x44); //set Ram-X address start/end position   
	Epaper_Write_Data(0x00);
	Epaper_Write_Data(0x18);    //0x0C-->(18+1)*8=200

	Epaper_Write_Command(0x45); //set Ram-Y address start/end position          
	Epaper_Write_Data(0x00);
	Epaper_Write_Data(0x00);
	Epaper_Write_Data(0xC7);    //0xC7-->(199+1)=200
	Epaper_Write_Data(0x00);

	Epaper_Write_Command(0x3C); //BorderWavefrom
	Epaper_Write_Data(0x05);

	Epaper_Write_Command(0x18); //Read built-in temperature sensor
	Epaper_Write_Data(0x80);

	Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
	Epaper_Write_Data(0x00);
	Epaper_Write_Command(0x4F);   // set RAM y address count to 0X199;    
	Epaper_Write_Data(0x00);
	Epaper_Write_Data(0x00);
	Epaper_READBUSY();

}
/////////////////////////////////////////////////////////////////////////////////////////
/*When the electronic paper screen is updated, do not unplug the electronic paper to avoid damage to the screen*/

void EPD_Update(void) {
	Epaper_Write_Command(0x22); //Display Update Control
	Epaper_Write_Data(0xF7);
	Epaper_Write_Command(0x20); //Activate Display Update Sequence
	Epaper_READBUSY();

}
/*When the electronic paper screen is updated, do not unplug the electronic paper to avoid damage to the screen*/
void EPD_Part_Update(void) {
	Epaper_Write_Command(0x22); //Display Update Control
	Epaper_Write_Data(0xFF);
	Epaper_Write_Command(0x20); //Activate Display Update Sequence
	Epaper_READBUSY();
}
//////////////////////////////All screen update////////////////////////////////////////////
void EPD_WhiteScreen_ALL(const unsigned char *datas) {
	unsigned int i;
	Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
	for (i = 0; i < 5000; i++) {
		Epaper_Write_Data(*datas);
		datas++;
	}
	EPD_Update();
}

void EPD_WhiteScreen_ALL2(const unsigned char *datas) {

	Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
	HAL_GPIO_WritePin(displayDataCommand_Port, displayDataCommand_Pin,
			GPIO_PIN_SET); // Select Data
	HAL_GPIO_WritePin(displayChipSelect_Port, displayChipSelect_Pin,
			GPIO_PIN_RESET); // Drive CS low = SPI slave selection
	uint8_t data;
	for (int yrow = 0; yrow < 200; yrow++) {
		for (int xcol = 0; xcol < 25; xcol++) {
			if (xcol == 0) {
				data = 0x3E;
			} else {
				data = 0xFF;
			}
		HAL_SPI_Transmit(&hspi2, &data, 1U, 20U);
		}
	}

	HAL_GPIO_WritePin(displayChipSelect_Port, displayChipSelect_Pin,
			GPIO_PIN_SET); // Drive CS low = end of SPI slave selection
	EPD_Update();
}

///////////////////////////Part update//////////////////////////////////////////////
//The x axis is reduced by one byte, and the y axis is reduced by one pixel.
void EPD_SetRAMValue_BaseMap(const unsigned char *datas) {
	unsigned int i;
	const unsigned char *datas_flag;
	datas_flag = datas;

	Epaper_Write_Command(0x24);   //Write Black and White image to RAM
	for (i = 0; i < 5000; i++) {
		Epaper_Write_Data(*datas);
		datas++;
	}
	datas = datas_flag;
	Epaper_Write_Command(0x26);   //Write Black and White image to RAM
	for (i = 0; i < 5000; i++) {
		Epaper_Write_Data(*datas);
		datas++;
	}
	EPD_Update();

}
void EPD_Dis_Part(unsigned int x_start, unsigned int y_start,
		const unsigned char *datas, unsigned int PART_COLUMN,
		unsigned int PART_LINE) {
	unsigned int i;
	unsigned int x_end, y_start1, y_start2, y_end1, y_end2;
	x_start = x_start / 8;
	x_end = x_start + PART_LINE / 8 - 1;

	y_start1 = 0;
	y_start2 = y_start;
	if (y_start >= 256) {
		y_start1 = y_start2 / 256;
		y_start2 = y_start2 % 256;
	}
	y_end1 = 0;
	y_end2 = y_start + PART_COLUMN - 1;
	if (y_end2 >= 256) {
		y_end1 = y_end2 / 256;
		y_end2 = y_end2 % 256;
	}

	Epaper_Write_Command(0x44);   // set RAM x address start/end, in page 35
	Epaper_Write_Data(x_start);    // RAM x address start at 00h;
	Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128
	Epaper_Write_Command(0x45);   // set RAM y address start/end, in page 35
	Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
	Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
	Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
	Epaper_Write_Data(y_end1);    // ????=0

	Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
	Epaper_Write_Data(x_start);
	Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;
	Epaper_Write_Data(y_start2);
	Epaper_Write_Data(y_start1);

	Epaper_Write_Command(0x24);   //Write Black and White image to RAM
	for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++) {
		Epaper_Write_Data(*datas);
		datas++;
	}
	EPD_Part_Update();

}

void EPD_DeepSleep(void) {
	Epaper_Write_Command(0x10); //enter deep sleep
	Epaper_Write_Data(0x01);
	HAL_Delay(100);
}

/////////////////////////////////Single display////////////////////////////////////////////////

void EPD_WhiteScreen_White(void)

{
	unsigned int i, k;
	Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
	for (k = 0; k < 200; k++) {
		for (i = 0; i < 25; i++) {
			Epaper_Write_Data(0xff);
		}
	}
	EPD_Update();
}

/////////////////////////////////////TIME///////////////////////////////////////////////////
void EPD_Dis_Part_myself(unsigned int x_startA, unsigned int y_startA,
		const unsigned char *datasA, unsigned int x_startB,
		unsigned int y_startB, const unsigned char *datasB,
		unsigned int x_startC, unsigned int y_startC,
		const unsigned char *datasC, unsigned int x_startD,
		unsigned int y_startD, const unsigned char *datasD,
		unsigned int x_startE, unsigned int y_startE,
		const unsigned char *datasE, unsigned int PART_COLUMN,
		unsigned int PART_LINE) {
	unsigned int i;
	unsigned int x_end, y_start1, y_start2, y_end1, y_end2;

//Data A////////////////////////////
	x_startA = x_startA / 8;   //Convert to byte
	x_end = x_startA + PART_LINE / 8 - 1;

	y_start1 = 0;
	y_start2 = y_startA - 1;
	if (y_startA >= 256) {
		y_start1 = y_start2 / 256;
		y_start2 = y_start2 % 256;
	}
	y_end1 = 0;
	y_end2 = y_startA + PART_COLUMN - 1;
	if (y_end2 >= 256) {
		y_end1 = y_end2 / 256;
		y_end2 = y_end2 % 256;
	}

// Add hardware reset to prevent background color change
	EPD_RESET();

//Lock the border to prevent flashing
	Epaper_Write_Command(0x3C); //BorderWavefrom,
	Epaper_Write_Data(0x80);

	Epaper_Write_Command(0x44);   // set RAM x address start/end, in page 35
	Epaper_Write_Data(x_startA);    // RAM x address start at 00h;
	Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128
	Epaper_Write_Command(0x45);   // set RAM y address start/end, in page 35
	Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
	Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
	Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
	Epaper_Write_Data(y_end1);

	Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
	Epaper_Write_Data(x_startA);
	Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;
	Epaper_Write_Data(y_start2);
	Epaper_Write_Data(y_start1);

	Epaper_Write_Command(0x24);   //Write Black and White image to RAM
	for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++) {
		Epaper_Write_Data(*datasA);
		datasA++;
	}
//Data B/////////////////////////////////////
	x_startB = x_startB / 8;   //Convert to byte
	x_end = x_startB + PART_LINE / 8 - 1;

	y_start1 = 0;
	y_start2 = y_startB - 1;
	if (y_startB >= 256) {
		y_start1 = y_start2 / 256;
		y_start2 = y_start2 % 256;
	}
	y_end1 = 0;
	y_end2 = y_startB + PART_COLUMN - 1;
	if (y_end2 >= 256) {
		y_end1 = y_end2 / 256;
		y_end2 = y_end2 % 256;
	}

	Epaper_Write_Command(0x44);   // set RAM x address start/end, in page 35
	Epaper_Write_Data(x_startB);    // RAM x address start at 00h;
	Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128
	Epaper_Write_Command(0x45);   // set RAM y address start/end, in page 35
	Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
	Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
	Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
	Epaper_Write_Data(y_end1);

	Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
	Epaper_Write_Data(x_startB);
	Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;
	Epaper_Write_Data(y_start2);
	Epaper_Write_Data(y_start1);

	Epaper_Write_Command(0x24);   //Write Black and White image to RAM
	for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++) {
		Epaper_Write_Data(*datasB);
		datasB++;
	}

//Data C//////////////////////////////////////
	x_startC = x_startC / 8;   //Convert to byte
	x_end = x_startC + PART_LINE / 8 - 1;

	y_start1 = 0;
	y_start2 = y_startC - 1;
	if (y_startC >= 256) {
		y_start1 = y_start2 / 256;
		y_start2 = y_start2 % 256;
	}
	y_end1 = 0;
	y_end2 = y_startC + PART_COLUMN - 1;
	if (y_end2 >= 256) {
		y_end1 = y_end2 / 256;
		y_end2 = y_end2 % 256;
	}

	Epaper_Write_Command(0x44);   // set RAM x address start/end, in page 35
	Epaper_Write_Data(x_startC);    // RAM x address start at 00h;
	Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128
	Epaper_Write_Command(0x45);   // set RAM y address start/end, in page 35
	Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
	Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
	Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
	Epaper_Write_Data(y_end1);

	Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
	Epaper_Write_Data(x_startC);
	Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;
	Epaper_Write_Data(y_start2);
	Epaper_Write_Data(y_start1);

	Epaper_Write_Command(0x24);   //Write Black and White image to RAM
	for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++) {
		Epaper_Write_Data(*datasC);
		datasC++;
	}

//Data D//////////////////////////////////////
	x_startD = x_startD / 8;   //Convert to byte
	x_end = x_startD + PART_LINE / 8 - 1;

	y_start1 = 0;
	y_start2 = y_startD - 1;
	if (y_startD >= 256) {
		y_start1 = y_start2 / 256;
		y_start2 = y_start2 % 256;
	}
	y_end1 = 0;
	y_end2 = y_startD + PART_COLUMN - 1;
	if (y_end2 >= 256) {
		y_end1 = y_end2 / 256;
		y_end2 = y_end2 % 256;
	}

	Epaper_Write_Command(0x44);   // set RAM x address start/end, in page 35
	Epaper_Write_Data(x_startD);    // RAM x address start at 00h;
	Epaper_Write_Data(x_end);      // RAM x address end at 0fh(15+1)*8->128
	Epaper_Write_Command(0x45);   // set RAM y address start/end, in page 35
	Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
	Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
	Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
	Epaper_Write_Data(y_end1);

	Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
	Epaper_Write_Data(x_startD);
	Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;
	Epaper_Write_Data(y_start2);
	Epaper_Write_Data(y_start1);

	Epaper_Write_Command(0x24);   //Write Black and White image to RAM
	for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++) {
		Epaper_Write_Data(*datasD);
		datasD++;
	}
//Data E//////////////////////////////////////
	x_startE = x_startE / 8;   //Convert to byte
	x_end = x_startE + PART_LINE / 8 - 1;

	y_start1 = 0;
	y_start2 = y_startE - 1;
	if (y_startE >= 256) {
		y_start1 = y_start2 / 256;
		y_start2 = y_start2 % 256;
	}
	y_end1 = 0;
	y_end2 = y_startE + PART_COLUMN - 1;
	if (y_end2 >= 256) {
		y_end1 = y_end2 / 256;
		y_end2 = y_end2 % 256;
	}

	Epaper_Write_Command(0x44);   // set RAM x address start/end, in page 35
	Epaper_Write_Data(x_startE);    // RAM x address start at 00h;
	Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128
	Epaper_Write_Command(0x45);   // set RAM y address start/end, in page 35
	Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
	Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
	Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
	Epaper_Write_Data(y_end1);

	Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
	Epaper_Write_Data(x_startE);
	Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;
	Epaper_Write_Data(y_start2);
	Epaper_Write_Data(y_start1);

	Epaper_Write_Command(0x24);   //Write Black and White image to RAM
	for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++) {
		Epaper_Write_Data(*datasE);
		datasE++;
	}
	EPD_Part_Update();

}

/***********************************************************
 end file
 ***********************************************************/

