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
const double TRAN_CM=1*340.0*100/72000000.0;
float dist_cm=0;
static uint32_t this_ticks = 0;
void EchoPrint()
{
	//FLAG=1;
	TOTAL=0;
}


void EXTI7_IRQHandler()
{
	uint32_t temp=SysTick->VAL;//fix the clock cycle
	if(temp-ULTRA_EMIT>10)
	{
			if(temp-ULTRA_EMIT>10)
			{
				OUT_NUM=temp-ULTRA_EMIT;//now sysclock-sysclock of sending signal
			}
			else
			{
				//OUT_NUM=temp+72000000-ULTRA_EMIT;
				OUT_NUM=temp;
			}
			
			//if(temp>1000000)//check out of bound by clock
			if(OUT_NUM>1000000)
			{
				OUT_NUM=1000000;
			}
			ULTRA_EMIT=SysTick->VAL;
			//OUT_NUM=temp;
			//translate into cm
			dist_cm=OUT_NUM*TRAN_CM;
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
	tft_prints(0,0,"hello",TOTAL);
	tft_update();
	
	us_init();
	//setReceive_listener(EchoPrint);//set interupt to receive ultrasonic
	gpio_exti_init(GPIO8,EXTI_Trigger_Rising_Falling);
	//gpio_exti_init(GPIO8,EXTI_Trigger_Rising);
	ULTRA_EMIT=SysTick->VAL;
	
	while (1) {
		
		
		//while (get_ticks() == this_ticks)
		while(SysTick->VAL==this_ticks)//the speed of code become 1/72,000,000
		{
			
		}
		//this_ticks =get_ticks(); 
		this_ticks =SysTick->VAL;//
		
		set_cycle(this_ticks);
		//this_ticks = SysTick->VAL;//get_ticks();
		
		
		if(FLAG)
		{
			tft_clear();
			tft_prints(0,0,"??? %lu \nDetected at distances:%f\n%f"	,OUT_NUM,dist_cm,TRAN_CM);
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
