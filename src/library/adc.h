#ifndef __ADC_H
#define __ADC_H

/*	
 *	Base ADC Library for 2018-19 Internal Mainboard
 *
 *  Author  : Anshuman Medhi
 *  Contact : amedhi@connect.ust.hk 
 *						68925193
 *
 */

#include "gpio.h"

typedef struct {
	const GPIOPin* pin; //GPIO Pin
	u8 index; //leave 0, filled during init
	u8 channel; //ADC Channel
} ADCPort;

/**
 * Predefined ADC Ports, Feel free to read the Schematic/Datasheet and create your own
 * ADC_IO_X refers to using the GPIO Port X in ADC mode
 */
extern ADCPort ADC_CH1,ADC_CH2,ADC_CH3,ADC_CH4,ADC_CH5,ADC_CH6,ADC_CH7,ADC_CH8;
#define ADC_IO_1 &ADC_CH1
#define ADC_IO_2 &ADC_CH2
#define ADC_IO_3 &ADC_CH3
#define ADC_IO_4 &ADC_CH4
#define ADC_IO_5 &ADC_CH5
#define ADC_IO_6 &ADC_CH6
#define ADC_IO_7 &ADC_CH7
#define ADC_IO_8 &ADC_CH8

/**
 * @brief      Initialize an ADC Channel
 *
 * @param      adc_port  The ADC Port
 */
void adc_channel_init(ADCPort* adc_port);

/**
 * @brief      Initialize the Whole ADC
 * @note       IMPORTANT: must be called after all the channels are initialized
 */
void adc_init(void);

/**
 * @brief      Gets the latest ADC reading corresponding to the given ADC Port
 *
 * @param      adc   The ADC Port
 *
 * @return     The ADC Reading
 */
u16 get_adc(ADCPort* adc_port);

#endif
