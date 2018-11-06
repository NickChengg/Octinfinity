#ifndef __BUZZER_H
#define __BUZZER_H

/*	
 *	Simple Buzzer Library for 2018-19 Internal Mainboard
 *	
 *	This buzzer has the great power to annoy everyone around you so remember:
 *	
 *		"With Great Power comes Great Responsibility"
 *												- Uncle Ben, R.I.P.
 *
 *  Author  : Anshuman Medhi
 *  Contact : amedhi@connect.ust.hk 
 *						68925193
 *
 */

#include "gpio.h"

/**
 * @brief      Initialize the Onboard Buzzer
 */
void buzzer_init(void);

/**
 * @brief      Start Buzzing
 */
void buzzer_on(void);

/**
 * @brief      Stop Buzzing
 */
void buzzer_off(void);

#endif
