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

const int cycle=100;//100 ms as cycle
int TOTAL=0;

void EchoPrint()
{
	tft_clear();
	tft_prints(0,0,"%d"	,TOTAL);
	tft_update();
	TOTAL=0;
}


int main() {
	// Initialize Everything Here
	rcc_init();
	ticks_init();
	oled_init();
	
	tft_init(0, WHITE, RED, GREEN, DARK_RED);
	tft_clear();
	
	us_init();
	setReceive_listener(EchoPrint);//set interupt to receive ultrasonic
	
	
	
	while (1) {
		static u32 this_ticks = 0;
		while (get_ticks() == this_ticks);
		this_ticks = get_ticks();

		TOTAL++;
		set_cycle(TOTAL,cycle);
		static u32 last_led_ticks=0;
		if ((this_ticks - last_led_ticks) >= 25) {
			last_led_ticks = this_ticks;
			//Code in here will run every 25ms
			
		}
	}
}
