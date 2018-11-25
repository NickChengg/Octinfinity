#include "sonic.h"
#include "gpio.h"	
#include "lcd_main.h"

#define US_TO_SYS 72.4637681159    						//1000.0/13.8;
#define US_TO_MM  0.17												//1e-6 * 340.0 / 2 * 1e3

GPIOPin trig_pin, echo_pin;
static u8 sonic_received = 0;

uint32_t sonic_dur, sonic_mm;
u32 rise = 0, fall = 0;

void sonic_init(const GPIOPin* trig, const GPIOPin* echo){
	trig_pin = *trig;
	echo_pin = *echo;
	gpio_init(GPIO7, GPIO_Mode_Out_PP);	 	//not sure about the init mode is correct
	gpio_init(GPIO8, GPIO_Mode_IPU);				//not sure about the init mode is correct
}

//regularly send pulse with fixed length
void sonic_cycle(){
	static u8 sonic_sent = 0;
	static u32 triggered_sysTicks;
	
	if(!sonic_sent){
		//Create trigger pulse of 10us (stay here)
		triggered_sysTicks = SysTick->VAL;
		gpio_write(GPIO7,1);
		while(SysTick->VAL - triggered_sysTicks < (10.0*US_TO_SYS));
		gpio_write(GPIO7,0);
		sonic_sent = 1;
	} else {
		//Wait for 60ms (pass until condition satisfied)
		if(SysTick->VAL - triggered_sysTicks>  + (60.0*1000.0+10.0)*US_TO_SYS){
			if(!sonic_received){	//no interrupt triggered, ie inf far
				sonic_dur = 50.0*1000.0* US_TO_SYS;
				sonic_mm = (double)sonic_dur * US_TO_MM * 10.0;			
			}else sonic_received = 0;
			triggered_sysTicks = SysTick->VAL;	
			sonic_sent = 0;
		}		
	}
}

void sonic_echo_handler(){
	//based on 2nd mechanism
	//placed in IRQHandler of EXTI_Trigger_Rising_Falling
	static u32 received_sysTicks;
	
	if(gpio_read(GPIO8)){		//rising
		received_sysTicks = SysTick->VAL;
		tft_prints(0,3,"rise:%d",(int)rise++);
	} else {										//falling
		tft_prints(0,4,"fall:%d",(int)fall++);
		sonic_dur = SysTick->VAL - received_sysTicks;
		sonic_mm = (double)sonic_dur / US_TO_SYS * US_TO_MM * 10.0;
		sonic_received = 1;
	}	
}
