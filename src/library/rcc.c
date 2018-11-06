#include "rcc.h"

#include "stm32f10x_rcc.h"

void rcc_init(void) {
	afio_rcc_init();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
} 

void gpio_rcc_init(GPIO_TypeDef* gpio){
	if (gpio==GPIOA) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	} else if (gpio==GPIOB) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	} else if (gpio==GPIOC) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	} else if (gpio==GPIOD) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	} else if (gpio==GPIOE) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	}
}

void uart_rcc_init(USART_TypeDef* uart) {
	if (uart==USART1) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	} else if (uart==USART2) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	} else if (uart==USART3) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	} else if (uart==UART4) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	} else if (uart==UART5) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
	}
}

void tim_rcc_init(TIM_TypeDef* tim) {
	if (tim==TIM1) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	} else if (tim==TIM8) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	} else if (tim==TIM15) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE);
	} else if (tim==TIM16) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);
	} else if (tim==TIM17) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17, ENABLE);
	} else if (tim==TIM9) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
	} else if (tim==TIM10) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);
	} else if (tim==TIM11) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11, ENABLE);
	} else if (tim==TIM2) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	} else if (tim==TIM3) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	} else if (tim==TIM4) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	} else if (tim==TIM5) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	} else if (tim==TIM6) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	} else if (tim==TIM7) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
	} else if (tim==TIM12) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);
	} else if (tim==TIM13) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);
	} else if (tim==TIM14) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
	}
} 

void spi_rcc_init(SPI_TypeDef* spi) {
	if (spi==SPI1) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	} else if (spi==SPI2) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	} else if (spi==SPI3) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	}
}

void adc_rcc_init(ADC_TypeDef* adc) {
	if (adc==ADC1) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	} else if (adc==ADC2) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
	} else if (adc==ADC3) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
	}
}

void dma_rcc_init(DMA_TypeDef* dma) {
	if (dma==DMA1) {
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	} else if (dma==DMA2) {
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	}
}

void afio_rcc_init(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}
