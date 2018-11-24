 #ifndef __GPIO_H
#define __GPIO_H

/**
 * Simple (but versatile) GPIO Configuration library 
 * 
 *  Author  : Anshuman Medhi
 *  Contact : amedhi@connect.ust.hk 
 *						68925193
 *
 */

#include "stm32f10x_gpio.h"	  

typedef struct {
	GPIO_TypeDef* port;
	u16 pin;
} GPIOPin;

/**
 * Handles for every GPIO Pin on the STM32F103VXXX
 */
static const GPIOPin
	/*** GPIOA ***/
	PA0 = {GPIOA, GPIO_Pin_0},
	PA1 = {GPIOA, GPIO_Pin_1},
	PA2 = {GPIOA, GPIO_Pin_2},
	PA3 = {GPIOA, GPIO_Pin_3},
	PA4 = {GPIOA, GPIO_Pin_4},
	PA5 = {GPIOA, GPIO_Pin_5},
	PA6 = {GPIOA, GPIO_Pin_6},
	PA7 = {GPIOA, GPIO_Pin_7},
	PA8 = {GPIOA, GPIO_Pin_8},
	PA9 = {GPIOA, GPIO_Pin_9},
	PA10 = {GPIOA, GPIO_Pin_10},
	PA11 = {GPIOA, GPIO_Pin_11},
	PA12 = {GPIOA, GPIO_Pin_12},
	PA13 = {GPIOA, GPIO_Pin_13},
	PA14 = {GPIOA, GPIO_Pin_14},
	PA15 = {GPIOA, GPIO_Pin_15},

	/*** GPIOB ***/
	PB0 = {GPIOB, GPIO_Pin_0},
	PB1 = {GPIOB, GPIO_Pin_1},
	PB2 = {GPIOB, GPIO_Pin_2},
	PB3 = {GPIOB, GPIO_Pin_3},
	PB4 = {GPIOB, GPIO_Pin_4},
	PB5 = {GPIOB, GPIO_Pin_5},
	PB6 = {GPIOB, GPIO_Pin_6},
	PB7 = {GPIOB, GPIO_Pin_7},
	PB8 = {GPIOB, GPIO_Pin_8},
	PB9 = {GPIOB, GPIO_Pin_9},
	PB10 = {GPIOB, GPIO_Pin_10},
	PB11 = {GPIOB, GPIO_Pin_11},
	PB12 = {GPIOB, GPIO_Pin_12},
	PB13 = {GPIOB, GPIO_Pin_13},
	PB14 = {GPIOB, GPIO_Pin_14},
	PB15 = {GPIOB, GPIO_Pin_15},

	/*** GPIOC ***/
	PC0 = {GPIOC, GPIO_Pin_0},
	PC1 = {GPIOC, GPIO_Pin_1},
	PC2 = {GPIOC, GPIO_Pin_2},
	PC3 = {GPIOC, GPIO_Pin_3},
	PC4 = {GPIOC, GPIO_Pin_4},
	PC5 = {GPIOC, GPIO_Pin_5},
	PC6 = {GPIOC, GPIO_Pin_6},
	PC7 = {GPIOC, GPIO_Pin_7},
	PC8 = {GPIOC, GPIO_Pin_8},
	PC9 = {GPIOC, GPIO_Pin_9},
	PC10 = {GPIOC, GPIO_Pin_10},
	PC11 = {GPIOC, GPIO_Pin_11},
	PC12 = {GPIOC, GPIO_Pin_12},
	PC13 = {GPIOC, GPIO_Pin_13},
	PC14 = {GPIOC, GPIO_Pin_14},
	PC15 = {GPIOC, GPIO_Pin_15},
	
	/*** GPIOD ***/
	PD0 = {GPIOD, GPIO_Pin_0},
	PD1 = {GPIOD, GPIO_Pin_1},
	PD2 = {GPIOD, GPIO_Pin_2};

/**
 * Convenience defines for the GPIO Ports on the 2018-19 Internal Mainboard
 */
#define GPIO1 &PA0
#define GPIO2 &PA1
#define GPIO3 &PA2
#define GPIO4 &PA3
#define GPIO5 &PA4
#define GPIO6 &PA5
#define GPIO7 &PA6
#define GPIO8 &PA7
#define GPIO9  &PA13
#define GPIO10 &PA14


/**
 * @brief      Initialize a GPIO Pin in a specific mode
 *
 * @param[in]  gpio  The GPIO Pin
 * @param[in]  mode  The Mode of GPIO pin
 */
static inline void gpio_init(const GPIOPin* gpio, GPIOMode_TypeDef mode) {
	// gpio_rcc_init(gpio->port);

	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = mode;
	GPIO_InitStructure.GPIO_Pin = gpio->pin;

	GPIO_Init(gpio->port, &GPIO_InitStructure);
}


/**
 * @brief      Initialize the external interrupt for a specific gpio pin
 * 
 * @NOTE the exti is split into lines where each of the 16 pins on one port is attached to a line.
 * however pin 1 on every port (A,B,C) is on the same line, some only one of them may be used
 *
 * @NOTE It will call EXTIXY_IRQHandler where XY is the pin number.
 *
 * @param[in]  gpio     The GPIO Pin
 * @param[in]  trigger  The trigger mode, Rising, Falling or Both
 */
void gpio_exti_init(const GPIOPin* gpio, EXTITrigger_TypeDef trigger);

/**
 * @brief      Read the input value of the GPIO Pin
 *
 * @param[in]  gpio  The GPIO Pin
 *
 * @return     { description_of_the_return_value }
 */
static inline u8 gpio_read(const GPIOPin* gpio) {
	return GPIO_ReadInputDataBit(gpio->port, gpio->pin);
}

/**
 * @brief      Write a value to the output of the GPIO Pin
 *
 * @param[in]  gpio  The GPIO Pin
 * @param[in]  bit   The output value
 */
static inline void gpio_write(const GPIOPin* gpio, u8 bit) {
	if (bit) GPIO_SetBits(gpio->port, gpio->pin);
	else     GPIO_ResetBits(gpio->port, gpio->pin);
}

/**
 * @brief      Quickly set the output of the GPIO Pin (High) functionally
 *             equivalent but sometimes faster than gpio_write(1)
 *
 * @param[in]  gpio  The GPIO Pin
 */
static inline void gpio_set(const GPIOPin* gpio) {
	GPIO_SetBits(gpio->port, gpio->pin);
}

/**
 * @brief      Quickly reset the output of the GPIO Pin (Low) functionally
 *             equivalent but sometimes faster than gpio_write(0)
 *
 * @param[in]  gpio  The GPIO Pin
 */
static inline void gpio_reset(const GPIOPin* gpio) {
	GPIO_ResetBits(gpio->port, gpio->pin);
}

#endif
