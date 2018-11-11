#include "ultrasonic.h"

#define send_PIN GPIO1
#define recevie_PIN GPIO2



static UltrasonicStruct receive_pin = {recevie_PIN, 0};

void us_init()
{
	gpio_init(send_PIN, GPIO_Mode_Out_PP);
	gpio_init(recevie_PIN, GPIO_Mode_IPD);
}

void set_send_signal()
{
	gpio_write(send_PIN,1);
}

void reset_send_signal()
{
	gpio_write(send_PIN,0);
}

void setReceive_listener(listener event)
{
	if(gpio_read(recevie_PIN))
	{
		receive_pin.receive_action=event;
	}
}

void set_cycle(int ticks, int cycle)
{
	if(ticks%cycle>(cycle*0.05)&&ticks%cycle<(2*cycle*0.05))
	{
		gpio_write(send_PIN,1);
	}
	else /*if(ticks%cycle<(2*cycle*0.05))*/
	{
		gpio_write(send_PIN,0);
	}
}