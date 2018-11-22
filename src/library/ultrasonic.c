#include "ultrasonic.h"
#include "gpio.h"	

#define echo_PIN GPIO7
#define trig_PIN GPIO8

typedef void (*listener)(void);

/*
typedef struct {
	const GPIOPin* pin;
	listener receive_action;
} UltrasonicStruct;
*/

void us_init();
void set_send_signal();
void reset_send_signal();
void setReceive_listener(listener event);
void set_cycle(u32 this_ticks);


static UltrasonicStruct TRIG_pin = {trig_PIN, 0};



void us_init()
{
	gpio_init(echo_PIN, GPIO_Mode_Out_PP);
	//gpio_init(trig_PIN, GPIO_Mode_IN_FLOATING);// no sure about the init mode is correct
	gpio_init(trig_PIN, GPIO_Mode_IPU);// no sure about the init mode is correct
	//gpio_init(trig_PIN, GPIO_Mode_IPD);// no sure about the init mode is correct
}

void set_send_signal()
{
	gpio_write(echo_PIN,1);
}

void reset_send_signal()
{
	gpio_write(echo_PIN,0);
}

void setReceive_listener(listener event)
{
		TRIG_pin.receive_action=event;
}

void set_cycle(u32 this_ticks)//set cycle got problem
{
	
	//1 cycle no receive
	if(SysTick->VAL-ULTRA_EMIT<1000)//1000clock cycle= 13.8us
	{
		set_send_signal();
	}
	else //if(ticks<=3*cycle)
	{
		reset_send_signal();
	}
	if(SysTick->VAL-ULTRA_EMIT>1000)//start to detect after 100 more cycle
	{
		if(SysTick->VAL-ULTRA_EMIT>1000000)
		{
			ULTRA_EMIT=SysTick->VAL;
			
		}
	}
	
	/*
	if(this_ticks%100==0)
	{
		set_send_signal();
		ULTRA_EMIT=SysTick->VAL;
	}
	else
	{
		reset_send_signal();
	}
	*/
}




int detect_signal()
{
	if(gpio_read(trig_PIN))
	{
		return 1;
	}
	return 0;
}

