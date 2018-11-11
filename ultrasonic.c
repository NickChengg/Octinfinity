#include "ultrasonic.h"

#define send_PIN GPIO1
#define recevie_PIN GPIO2


typedef void (*listener)(void);

typedef struct {
	const GPIOPin* pin;
	listener receive_action;
} UltrasonicStruct;

static UltrasonicStruct receive_pin = {recevie_PIN, 0};

void us_init()
{
	gpio_init(send_PIN, GPIO_Mode_Out_PP);
	gpio_init(recevie_PIN, GPIO_Mode_IPD);
}

int set_send_signal()
{
	gpio_write(send_PIN,1);
}

int reset_send_signal()
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