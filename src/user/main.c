#define LT_F GPIO1;
#define LT_L GPIO2;
#define LT_R GPIO3;
#define LT_B GPIO4;

#include "main.h"


// Include Library Headers Here

#include "rcc.h"
#include "ticks.h"
#include "gpio.h"
#include "lcd_main.h"

int main(){
// Initialize Everything Here
rcc_init();
ticks_init();
gpio_init(LT_F, GPIO_Mode_IPU);
gpio_init(LT_L, GPIO_Mode_IPU);
gpio_init(LT_R, GPIO_Mode_IPD);
gpio_init(LT_B, GPIO_Mode_IPD);
tft_init(PIN_ON_TOP, WHITE, BLACK, BLUE, RED);

while (1) {
  static u32 this_ticks = 0;
  static u32 last_ticks_50 = 0;

  static u8 lt_f, lt_l, lt_r, lt_b;
  while (get_ticks() == this_ticks);
  this_ticks = get_ticks();

  // every 50ms
  if (this_ticks - last_ticks_50 >= 1) { 
    last_ticks_50 = this_ticks;
    lt_f = gpio_read(LT_F);
    lt_l = gpio_read(LT_L);
    lt_r = gpio_read(LT_R);
    lt_b = gpio_read(LT_B);
    tft_clear();
    tft_prints(3, 0, "F %d", lt_f);
    tft_prints(0, 1, "L %d", lt_l);
    tft_prints(6, 1, "R %d", lt_r);
    tft_prints(3, 2, "B %d", lt_b);
    tft_update();
  }
}
