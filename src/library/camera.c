/*	
 *	Camera library - internal 2017/18
 *	Mode 		: ov7725
 *  Author  : Emmett Yim
 *  Contact : yhyim@ust.hk 
 *						66816823
 *	Lib			: camera.c
 */

#include "camera.h"
#include "pwm.h"
#include "stm32f10x_tim.h"
#include "rcc.h"
#include "ov7725define.h"

ImageType imageType = RGBColour;
volatile uint8_t camState = 0;
uint8_t image[ImageLength][ImageWidth];
uint16_t colourImage[ImageLength][ImageWidth];
uint16_t imageToPrint[ImageLength][ImageWidth];

static DMA_InitTypeDef camera_printing_dma;

//ov7725
uint8_t camera_init(ImageType type) {	
	//sccb init
	if(!cameraSccbInit(type))
		return ERROR;

	//camera status init
	camState = 0;
	//set camera image type
	imageType = type;

	//fifo init
	gpio_init(FIFO_OE, GPIO_Mode_Out_PP);
	gpio_init(FIFO_RRST, GPIO_Mode_Out_PP);
	gpio_init(FIFO_WEN, GPIO_Mode_Out_PP);
	gpio_init(FIFO_WRST, GPIO_Mode_Out_PP);
	gpio_init(FIFO_RCLK, GPIO_Mode_Out_PP);
	gpio_init(FIFO_VSYNC, GPIO_Mode_IN_FLOATING);
	
	//		Data
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = FIFO_DMASK;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(FIFO_DPORT, &GPIO_InitStructure);
	
	gpio_exti_init(FIFO_VSYNC, EXTI_Trigger_Falling);

	// //	NVIC
	// NVIC_InitTypeDef NVIC_InitStructure;
	// NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	// NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	// NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	// NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	// NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	// NVIC_Init(&NVIC_InitStructure);
	// //	EXTI
	// EXTI_InitTypeDef EXTI_InitStructure;
	// GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource12);
	// EXTI_InitStructure.EXTI_Line = EXTI_Line12;
	// EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	// EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
	// EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	// EXTI_Init(&EXTI_InitStructure);
	// EXTI_GenerateSWInterrupt(EXTI_Line12);

	FIFO_OE_L();
	FIFO_WEN_H();

	dma_rcc_init(DMA1);

	//TFT DMA

	DMA_DeInit(DMA1_Channel5);
	camera_printing_dma.DMA_PeripheralBaseAddr = (u32) &TFT_SPI.periph->DR;
	camera_printing_dma.DMA_MemoryBaseAddr = (u32) &imageToPrint[0][0];
	camera_printing_dma.DMA_DIR = DMA_DIR_PeripheralDST;
	camera_printing_dma.DMA_BufferSize = ImageLength*ImageWidth*2;
	camera_printing_dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	camera_printing_dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
	camera_printing_dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	camera_printing_dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	camera_printing_dma.DMA_Mode = DMA_Mode_Normal;
	camera_printing_dma.DMA_Priority = DMA_Priority_High;
	camera_printing_dma.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel5, &camera_printing_dma);

	SPI_I2S_DMACmd(TFT_SPI.periph, SPI_I2S_DMAReq_Tx, ENABLE);

	return SUCCESS;
}


uint8_t camera_ready() {
	return camState == 2;
}

uint8_t camera_receive_frame(void) {
	if(camState != 2)
		return ERROR;
	
	FIFO_READY;

	if(imageType == RGBColour) {
		for (u16* ptr = &colourImage[0][0]; ptr != &colourImage[59][80];) {
			READ_FIFO_COLOUR(ptr);
		}
	}
	else if (imageType == GreyScale) {
		for (u8* ptr = &image[0][0]; ptr != &image[59][80];) {
			READ_FIFO_GREYSCALE(*ptr++);
		}
	}		

	camState = 0;

	return SUCCESS;
}

static inline uint16_t rotate(uint16_t input) {
	return (input>>8) | (input << 8);
}
uint16_t grey_to_rgb565(uint8_t input){
	return ((input << 8) & 0xF800) | ((input << 3) & 0x07E0) | (input >> 3);
}
static inline uint16_t grey_to_rgb565_r(uint8_t input) {
	return (input & 0xF8) | ((input >> 5) & 0x07) | ((input << 11) & 0xE000) | ((input << 5) & 0xFF00);
	// return rotate(grey_to_rgb565(input));
}

void camera_copy() {
	if(imageType == RGBColour) {
		u16* ptr2 = &colourImage[0][0];
		for (u16* ptr=&imageToPrint[0][0]; ptr!=&imageToPrint[59][80];) {
			// *ptr++ = rotate(*ptr2++);
			COPY_PX_COLOUR(*ptr2++, *ptr++);
		}
	} else if(imageType == GreyScale) {
		u8* ptr2 = &image[0][0];
		for (u16* ptr=&imageToPrint[0][0]; ptr!=&imageToPrint[59][80];) {
			// *ptr++ = grey_to_rgb565_r(*ptr2++);
			COPY_PX_GREYSCALE(*ptr2++, *ptr++);
		}
	}
}

void camera_print(uint8_t x, uint8_t y) {
	// while (!camera_printing_done());
	tft_set_region(x, y, ImageWidth-1, ImageLength-1);
	// camera_copy();
	DMA_DeInit(DMA1_Channel5);
	DMA_Init(DMA1_Channel5, &camera_printing_dma);
	DMA_Cmd(DMA1_Channel5, ENABLE);
}

uint8_t camera_printing_done() {
	return DMA_GetFlagStatus(DMA1_FLAG_TC5);
}

void EXTI12_IRQHandler(void) {
	if(camState == 0)
	{
		FIFO_WRST_L();
		FIFO_WEN_L();
		camState = 1;	   	
		FIFO_WEN_H();
		FIFO_WRST_H();
	}
	else if(camState == 1)
	{
		FIFO_WEN_L();
		camState = 2;
	}
}
