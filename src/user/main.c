/*	
 *	UST Robotics Team Internal 2018
 *
 *  Author  : Anshuman Medhi
 *  Contact : amedhi@connect.ust.hk 
 *						68925193
 *
 */

#include "main.h"

// Include Library Headers Here
#include "math.h"
#include "rcc.h"
#include "ticks.h"
#include "gpio.h"
#include "leds.h"
#include "buttons.h"
#include "buzzer.h"
#include "uart.h"
#include "lcd_main.h"
#include "oled.h"
#include "camera.h"
#include "pwm.h"
#include "adc.h"
#include "ultrasonic.h"

u16 mag_FL = 0, mag_FR = 0, mag_BL = 0, mag_BR = 0; //magnetic reading in (front/back)(left/right)
u16 servo_OC = (int)(5000 * 1.5 /20); //0.9 for leftmost, 2.1 for rightmost



void mag_read() {
	mag_FL = get_adc(ADC_IO_1);
	mag_FR = get_adc(ADC_IO_2);
	mag_BL = get_adc(ADC_IO_3);
	mag_BR = get_adc(ADC_IO_4);
}

int mag_OC_cal(int L_reading, int R_reading) { //left right reading from magnetic sensors
	const u32 max_reading = 1200; //max reading of sensor(closest), test! 
	const u32 min_reading = 300; //min reading of sensor(farest), i.e. the value of background noise, test!
	const float sensitivity = 1; //0-1, increse to turn more, vice versa
	s16 turning_percentage = 0; //turning percentage for servo, 100 for 100% of turning; -ve = left, +ve = right

	if (ABS(mag_FR - mag_FL) * sensitivity < (max_reading - min_reading)) {
		turning_percentage = (float)(R_reading - L_reading) / (max_reading - min_reading) * 100 * sensitivity;
	}
	else { // fix the range within -100 and 100
		turning_percentage = R_reading > L_reading ? 100 : -100;
	}
	
	return (int)(5000 * (1.5 + turning_percentage * 0.006) / 20);
}

void mag_compare() {
	const u32 acceptable_diff = 0; //turn only when excess acceptable difference, test! really need??

	if (ABS(mag_FL - mag_FR) > acceptable_diff) {
		servo_OC = mag_OC_cal(mag_FL, mag_FR);
	}
	else if (ABS(mag_BL - mag_BR) > acceptable_diff) {
		servo_OC = mag_OC_cal(mag_BL, mag_BR);
	}
}


int main() {
	// Initialize Everything Here
	rcc_init();
	ticks_init();
	oled_init();
	
	
	
	
	while (1) {
		static u32 this_ticks = 0;
		while (get_ticks() == this_ticks);
		this_ticks = get_ticks();

		static u32 last_led_ticks=0;
		if ((this_ticks - last_led_ticks) >= 25) {
			last_led_ticks = this_ticks;
			//Code in here will run every 25ms
			
		}
	}
}
