#ifndef __LEDS_H
#define __LEDS_H

/**
 * Simple LED library for 2018-19 Internal Mainboard 
 * 
 *  Author  : Anshuman Medhi
 *  Contact : amedhi@connect.ust.hk 
 *						68925193
 *
 */

#include "gpio.h"

/**
 * Defines for the onboard LEDS
 */
#define LED1 &PC1
#define LED2 &PC2
//LEDs are simple controlled as GPIO
typedef const GPIOPin* LED;

/**
 * @brief      Initialize all LEDS On the 2018-19 Internal Mainboard
 */
void leds_init(void);

//HINT: What about a toggle function? or a set function? or a blink function? be creative
/**
 * @brief      Turn on the LED
 *
 * @param[in]  led   The LED
 */
void led_on(LED led);

/**
 * @brief      Turn off the LED
 *
 * @param[in]  led   The LED
 */
void led_off(LED led);

#endif
