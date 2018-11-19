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
long int set_cycle(int ticks, int cycle);

int FLAG=0;
long int OUT_NUM=0;