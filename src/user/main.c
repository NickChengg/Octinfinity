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

const int cycle=5;//in ms, 1 cycle for signal, 5 cycle after receive echo
int TOTAL=0;
static long int output= 0;

/*
void EXTI8_IRQHandler()
{
	uint32_t temp=SysTick->VAL-ULTRA_EMIT;
			ULTRA_EMIT=SysTick->VAL;
			OUT_NUM=temp;
	FLAG=1;
}
*/
void EchoPrint()
{
	//FLAG=1;
	TOTAL=0;
}


void EXTI7_IRQHandler()
{
	
		//if(gpio_read(trig_PIN))
		{
			uint32_t temp=SysTick->VAL-ULTRA_EMIT;
			ULTRA_EMIT=SysTick->VAL;
			OUT_NUM=temp;
			FLAG=1;
		}
	
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
	gpio_exti_init(GPIO8,EXTI_Trigger_Rising_Falling);
	ULTRA_EMIT=SysTick->VAL;
	
	while (1) {
		static u32 this_ticks = 0;
		
		
		
		
		
		//while (get_ticks() == this_ticks)
		while(SysTick->VAL==this_ticks)
		{
			
		}
		output = set_cycle(0,cycle);
		this_ticks = SysTick->VAL;//get_ticks();
		
		/*
		if(temp)
		{
			if(temp<0)
			{
				temp+=72000000;
			}
			tft_clear();
			tft_prints(0,0,"detected at %d"	,temp);
			tft_update();
			temp=0;
		}
		*/
		
		if(FLAG)
		{
			tft_clear();
			tft_prints(0,0,"??? %d"	,OUT_NUM);
			tft_update();
			FLAG=0;
		}
		
		//long int reflection=set_cycle(TOTAL,cycle);
		
		static u32 last_led_ticks=0;
		if ((this_ticks - last_led_ticks) >= 25) {
			last_led_ticks = this_ticks;
			
		
		

			//Code in here will run every 25ms
			
		}
	}
}
