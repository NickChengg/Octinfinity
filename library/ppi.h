#ifndef __PPI_H
#define __PPI_H

/**
 * GPIO (and Timer) Bit-Banged Parallel I/O Interface
 */

typedef enum {
	PPITxMode, PPIRxMode
} PPIMode;

typedef enum {
	PPIClockRising, PPIClockFalling,
} PPIClockMode;

typedef struct {
	const GPIOPin* clk;
	GPIO_TypeDef* dport;
	u16           dpins;
	PPIMode mode;
	PPIClockMode clk_mode;
} PPIPort;

typedef struct {
	PPIPort base; //APPIPort extends PPIPort
	TIM_TypeDef* tim;
	u32 remap;
	u16 tim_psc;
	u8 tim_ch;
} APPIPort;

typedef struct {
	APPIPort base; //DMAPPIPort extends APPIPort
	DMA_TypeDef* dma;
	DMA_Channel_TypeDef* dma_ch;
} DMAPPIPort;

#endif
