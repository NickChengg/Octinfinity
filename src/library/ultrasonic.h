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

static int FLAG=0;
static u32 OUT_NUM=0;
static uint32_t ULTRA_EMIT=0;//used to store hardware emit signal time