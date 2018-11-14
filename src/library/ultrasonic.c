#include "ultrasonic.h"

#define send_PIN GPIO7
#define trig_PIN GPIO8

#include "gpio.h"	 

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
long int set_cycle(int ticks, int cycle);


static UltrasonicStruct TRIG_pin = {trig_PIN, 0};
long int ULTRA_EMIT=0;//used to store hardware emit signal time

void us_init()
{
	gpio_init(send_PIN, GPIO_Mode_Out_PP);
	//gpio_init(trig_PIN, GPIO_Mode_IN_FLOATING);// no sure about the init mode is correct
	gpio_init(trig_PIN, GPIO_Mode_IPU);// no sure about the init mode is correct
}

void set_send_signal()
{
	gpio_set(send_PIN);
}

void reset_send_signal()
{
	gpio_reset(send_PIN);
}

void setReceive_listener(listener event)
{
		TRIG_pin.receive_action=event;
}

long int set_cycle(int ticks, int cycle)//set cycle got problem
{
	//1 cycle no receive
	if(SysTick->VAL-ULTRA_EMIT<5)//send signal in 5 clock
	{
		gpio_write(send_PIN,1);
	}
	else //if(ticks<=3*cycle)
	{
		gpio_write(send_PIN,0);
	}
	if(ticks>10*cycle)
	{
		if(gpio_read(trig_PIN))
		{
			long int temp=SysTick->VAL-ULTRA_EMIT;
			ULTRA_EMIT=SysTick->VAL;
			return temp;
		}
	}
	return 0;
	
}

int detect_signal()
{
	if(gpio_read(trig_PIN))
	{
		return 1;
	}
	return 0;
}
