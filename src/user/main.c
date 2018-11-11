//#define LT_F GPIO1;
//#define LT_L GPIO2;
//#define LT_R GPIO3;
//#define LT_B GPIO4;

#include "main.h"
#include <string.h>
#include <stdio.h>
// Include Library Headers Here
#include "rcc.h"
#include "ticks.h"
#include "gpio.h"
#include "leds.h"
#include "buttons.h"
#include "buzzer.h"
#include "uart.h"
#include "lcd_main.h"
#include "oled.h"
#include "camera.h"
#include "pwm.h"
#include "adc.h"


int main(){
// Initialize Everything Here
rcc_init();
ticks_init();
gpio_init(GPIO5, GPIO_Mode_IPU);
gpio_init(GPIO6, GPIO_Mode_IPU);
gpio_init(GPIO7, GPIO_Mode_IPD);
gpio_init(GPIO8, GPIO_Mode_IPD);
tft_init(0, BLUE, RED, BLUE, RED);

while (1) {
  static u32 this_ticks = 0;
  static u32 last_ticks_50 = 0;

  static u8 lt_f, lt_l, lt_r, lt_b;
  while (get_ticks() == this_ticks);
  this_ticks = get_ticks();

  // every 50ms
  if (this_ticks - last_ticks_50 >= 50) { 
    last_ticks_50 = this_ticks;
    lt_f = gpio_read(GPIO5);
    lt_l = gpio_read(GPIO6);
    lt_r = gpio_read(GPIO7);
    lt_b = gpio_read(GPIO8);
    tft_clear();
		
		for(u8 i=32; i<64; ++i)
			for(u8 j=0; j<32; ++j)
				if(lt_f) tft_put_pixel(i,j,BLACK);
				else tft_put_pixel(i,j,WHITE);
		for(u8 i=0; i<32; ++i)
			for(u8 j=32; j<64; ++j)
				if(lt_l) tft_put_pixel(i,j,BLACK);
				else tft_put_pixel(i,j,WHITE);
		for(u8 i=64; i<96; ++i)
			for(u8 j=32; j<64; ++j)
				if(lt_r) tft_put_pixel(i,j,BLACK);
				else tft_put_pixel(i,j,WHITE);
		for(u8 i=32; i<64; ++i)
			for(u8 j=64; j<96; ++j)
				if(lt_b) tft_put_pixel(i,j,BLACK);
				else tft_put_pixel(i,j,WHITE);
    /*tft_prints(5, 1, "F", lt_f);
    tft_prints(1, 5, "L", lt_l);
    tft_prints(9, 5, "R", lt_r);
    tft_prints(5, 9, "B", lt_b);*/
    tft_update();
  }
}
}
