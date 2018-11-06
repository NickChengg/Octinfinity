#ifndef __RCC_H
#define __RCC_H

#include "stm32f10x.h"

/**
 * @brief      Initialize basic, necesary RCCs (GPIO and AFIO) 
 */
void rcc_init(void);

/**
 * @brief      Initialize the Reset and Clock Circuitry for a given peripheral
 *
 * @param      The peripheral
 */
void gpio_rcc_init(GPIO_TypeDef* gpio);
void uart_rcc_init(USART_TypeDef* uart);
void tim_rcc_init(TIM_TypeDef* tim);
void spi_rcc_init(SPI_TypeDef* spi);
void adc_rcc_init(ADC_TypeDef* adc);
void dma_rcc_init(DMA_TypeDef* dma);
void afio_rcc_init(void);

#endif
