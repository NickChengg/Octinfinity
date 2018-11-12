#include "ultrasonic.h"

#define send_PIN GPIO7
#define recevie_PIN GPIO8

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
int set_cycle(int ticks, int cycle);


static UltrasonicStruct receive_pin = {recevie_PIN, 0};

void us_init()
{
	gpio_init(send_PIN, GPIO_Mode_Out_PP);
	gpio_init(recevie_PIN, GPIO_Mode_IN_FLOATING);
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
		receive_pin.receive_action=event;
}

int set_cycle(int ticks, int cycle)//set cycle got problem
{
	if(ticks%cycle>(cycle/20)&&ticks%cycle<(cycle/10))
	{
		set_send_signal();
	}
	else /*if(ticks%cycle<(2*cycle*0.05))*/
	{
		reset_send_signal();
	}
	
	if(gpio_read(recevie_PIN)&&ticks%cycle<(cycle/5))
	{
		return 1;
	}
	return 0;
	
}

int detect_signal()
{
	if(gpio_read(recevie_PIN))
	{
		return 1;
	}
	return 0;
}
