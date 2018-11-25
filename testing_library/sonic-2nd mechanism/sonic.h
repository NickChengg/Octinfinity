	#ifndef __SONIC_H
	#define __SONIC_H

	#include "gpio.h"	 

	extern uint32_t sonic_dur, sonic_mm;
	extern u32 rise, fall;

	/**
	 * @brief      Initialize ultrasonic sensors
	 *
	 * @param      trig      output pin sending sonic
	 * @param      echo		   input pin receiving sonic) 
	 *
	 * @note			 pin names swapped compared to previous version
	 */
	void sonic_init(const GPIOPin* trig, const GPIOPin* echo);

	/**
	 * @brief      Regularly send pulse with fixed length
	 *
	 * @note			 execute once every shortest cycle (precision: sysTicks)
	 */
	void sonic_cycle(void);

	/**
	 * @brief      Calculate distance based on 2nd mechanism (duration of high-level pulse)
	 *
	 * @note			 placed in IRQHandler (mode: EXTI_Trigger_Rising_Falling)
	 */
	void sonic_echo_handler(void);

	#endif
