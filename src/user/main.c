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

const int cycle=50;//in ms, 1 cycle for signal, 5 cycle after receive echo
int TOTAL=0;
int FLAG=0;

void EchoPrint()
{
	FLAG=1;
	TOTAL=0;
}


int main() {
	// Initialize Everything Here
	rcc_init();
	ticks_init();
	oled_init();
	
	tft_init(0, WHITE, RED, GREEN, DARK_RED);
	tft_clear();
	tft_prints(0,0,"hello"	,TOTAL);
	tft_update();
	
	us_init();
	setReceive_listener(EchoPrint);//set interupt to receive ultrasonic
	
	
	
	while (1) {
		static u32 this_ticks = 0;
		while (get_ticks() == this_ticks);
		this_ticks = get_ticks();

		TOTAL++;
		if(set_cycle(TOTAL,cycle))
		{
			tft_clear();
			tft_prints(0,0,"no %d"	,TOTAL);
			tft_update();
			TOTAL=0;
		}
		
		if(TOTAL>20*cycle)
		{
			TOTAL=0;
		}
		
		if(FLAG)
		{
			tft_clear();
			tft_prints(0,0,"???"	,TOTAL);
			tft_update();
		}
		
		static u32 last_led_ticks=0;
		if ((this_ticks - last_led_ticks) >= 25) {
			last_led_ticks = this_ticks;
			//Code in here will run every 25ms
			
		}
	}
}
