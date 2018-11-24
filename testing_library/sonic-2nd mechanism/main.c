/*********************************
	Ultrasonic Library Demo
*********************************/
 
#include "gpio.h"
#include "ticks.h"
#include "main.h"
#include "rcc.h"
#include "leds.h"
#include "uart.h"
#include "lcd_main.h"
#include "pwm.h"
#include "adc.h"
#include "math.h"

#include "sonic.h"

#define TRIG_PIN GPIO7
#define ECHO_PIN GPIO8

static u32 this_ticks = 0, this_sys = 0;
static u32 last_ticks_25=0;
static u32 count = 0;

void EXTI7_IRQHandler(){
	sonic_echo_handler();
}

int main() {
	// Initialize Modules
	rcc_init();
	ticks_init();
	leds_init();
	tft_init(0, BLUE, WHITE, GREEN, DARK_RED);
	sonic_init(TRIG_PIN, ECHO_PIN);
	gpio_exti_init(ECHO_PIN, EXTI_Trigger_Rising_Falling);
	
	tft_clear();
	tft_prints(0, 0, "Initialized");
	tft_update();
	
	while (1) {
		//per smallest time unit
		this_sys = SysTick->VAL;
		sonic_cycle();
		
		this_ticks = get_ticks();
		
		//per 25 ticks
		if(this_ticks - last_ticks_25 > 25){
			last_ticks_25 = this_ticks;
			tft_clear();
			tft_prints(0,0,"Dur:%d\nmm:%f\nCnt:%d",sonic_dur,(double)sonic_mm,(int)count++);
			tft_update();
		}	
	}
}

