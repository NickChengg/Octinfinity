#include "gpio.h"	 

typedef void (*listener)(void);

typedef struct {
	const GPIOPin* pin;
	listener receive_action;
} UltrasonicStruct;

void us_init();
void set_send_signal();
void reset_send_signal();
void setReceive_listener(listener event);
int set_cycle(int ticks, int cycle);