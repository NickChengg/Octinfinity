//#define LT_F GPIO1;
//#define LT_L GPIO2;
//#define LT_R GPIO3;
//#define LT_B GPIO4;

#include "main.h"
#include <string.h>
#include <stdio.h>
// Include Library Headers Here
/*******************************************

				   	 # Octinfinity #
			  	Line Tracking Algorithm

											Carefully crafted by
														Edwin Ng Ka Ho

********************************************
[TODO]
0. test current codes  
1. test inverse trig function and type casting
2. test sqrt function
3. fully distinguish all cases
4. count grids
5. hard code path
6. varied grid width
********************************************
[Reminder]
1. sqrt: int32_t -> uint32_t (scaled by 1024)
2. atan: int32_t (scaled by 32768) -> int32_t (scaled by 100)
3. s32 = int32_t = signed int
	 u32 = uint32_t = unsigned int
*******************************************/

//Definition
#define LT_F &PB11
#define LT_L &PB10
#define LT_R GPIO5
#define LT_B GPIO6

#define H_HEIGHT 20 //mm	//half height
#define H_WIDTH 40 //mm		//half width
#define H_GRID 467.5 //mm	//half grid width

#define LT_UPPERTOLERANCE 3
#define LT_LOWERTOLERANCE 0.33

#define PI 3.14159265

//Headers
#include "main.h"

#include "rcc.h"
#include "ticks.h"
#include "gpio.h"
#include "lcd_main.h"

#include "oled.h"
#include "camera.h"
#include "pwm.h"
#include "adc.h"


#include <math.h>

static u32 this_ticks = 0;
  static u32 last_ticks_50 = 0;

  static u8 lt_f, lt_l, lt_r, lt_b;
	
typedef enum {
	DIR_F,
	DIR_L,
	DIR_R,
	DIR_B
} LT_DIRECTION;

typedef enum {
	PATTERN_FLRB = 36, PATTERN_FLBR = 52, PATTERN_FRLB = 24,
	PATTERN_FRBL = 56, PATTERN_FBLR = 28, PATTERN_FBRL = 44,
	PATTERN_LFRB = 33, PATTERN_LFBR = 49, PATTERN_LRFB = 9,
	PATTERN_LRBF = 57, PATTERN_LBFR = 13, PATTERN_LBRF = 45,
	PATTERN_RFLB = 12, PATTERN_RFBL = 50, PATTERN_RLFB = 6,
	PATTERN_RLBF = 54, PATTERN_RBFL = 14, PATTERN_RBLF = 30,
	PATTERN_BFLR = 19, PATTERN_BFRL = 35, PATTERN_BLFR = 7,
	PATTERN_BLRF = 39, PATTERN_BRFL = 11, PATTERN_BRLF = 27
} LT_PATTERN;

//TODO Interrupt

double arctan(double input){
	double output = input;
	for(u8 i=3; i<=11; i+=2){
		double temp = 1;
		for(u8 j=0; j<i; ++j) temp*=input;
		if((i-1)%4) output-=temp/i; else output+=temp/i;
	}
	return output/PI * 180.0;
}

int main(){
// Initialize Everything Here
	// Hardware Initialization
	rcc_init();
	ticks_init();
	gpio_init(GPIO5, GPIO_Mode_IPU);
	gpio_init(GPIO6, GPIO_Mode_IPU);
	gpio_init(GPIO7, GPIO_Mode_IPD);
	gpio_init(GPIO8, GPIO_Mode_IPD);
	gpio_init(LT_F, GPIO_Mode_IPU);
	gpio_init(LT_L, GPIO_Mode_IPU);
	gpio_init(LT_R, GPIO_Mode_IPD);
	gpio_init(LT_B, GPIO_Mode_IPD);
	tft_init(PIN_ON_TOP, WHITE, BLACK, BLUE, RED);
	tft_clear();
	
	// Variables Initialization
	u32 last_ticks_5 = 0;

	u8 lt_value[4], lt_last_value[4] = {0};
	u8 lt_pattern=0;
	u32 lt_basetime, lt_deltime[3];										//"del" = delta
	u8 lt_stage = 0;
	double tandeltheta, deltheta, delx;
	s32 delscaledtheta;
	double ratio_discrepancy; //temp
	
	char translate_dir[4] = {'F', 'L', 'R', 'B'};
	


while (1) {
  
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

  
	
	// Main Program
  while (1) {
    while (get_ticks() == this_ticks);
    this_ticks = get_ticks();

    if(this_ticks - last_ticks_5 >= 5){
      last_ticks_5 = this_ticks;
			
			//Read values from sensors
			lt_value[DIR_F] = gpio_read(LT_F);
			lt_value[DIR_L] = gpio_read(LT_L);
			lt_value[DIR_R] = gpio_read(LT_R);
			lt_value[DIR_B] = gpio_read(LT_B);
			
			//Append and display pattern, time
			for(u8 i=0; i<4; ++i){
				//If direction i, reading changes from 0 to 1,
				if(lt_value[i]==1 && lt_last_value[i]==0){
					lt_last_value[i] = 1;
					if(lt_stage<3) lt_pattern += i<<(2*lt_stage);
					if(lt_stage==0){
						lt_basetime = this_ticks;
				  }else{
						lt_deltime[lt_stage-1] = this_ticks - lt_basetime;
						tft_prints(2, 2 + lt_stage-1, "%d", lt_deltime[lt_stage-1]);
					}
					tft_prints(0, 2 + lt_stage, "%c", translate_dir[i]);
					lt_stage++;
				}
			}
			
			//Reset stage if all sensors no signals
			if(!(lt_value[0] || lt_value[1] || lt_value[2] || lt_value[3])) lt_stage = 0;
			
			//Determine pattern, calculate deviation
				//TODO Fully Categorize cases
			if(lt_stage==4){
				tft_prints(7, 3, "%d", lt_pattern); 
				switch(lt_pattern){
					case PATTERN_FLRB:
						//check ffff
						tandeltheta = (double)(H_HEIGHT*((s32)lt_deltime[0]-(s32)lt_deltime[1]))/(double)(H_WIDTH*((s32)lt_deltime[0]+(s32)lt_deltime[1]));
						ratio_discrepancy = ((double)lt_deltime[0]/(double)lt_deltime[2]) / ((double)(H_HEIGHT + H_WIDTH * tandeltheta)/(double)(2 * H_HEIGHT));
						tft_prints(7, 4, "%f", ratio_discrepancy); 	
						if(LT_LOWERTOLERANCE < ratio_discrepancy && ratio_discrepancy < LT_UPPERTOLERANCE){
							//confirmed ffff
							deltheta = arctan(tandeltheta);
							tft_prints(0,6,"tan %f", tandeltheta);
							tft_prints(0, 7, "ffff %f", deltheta); 
						}else{
							//check flff
							tandeltheta = (double)H_HEIGHT * (lt_deltime[2] - 2 * lt_deltime[1]) / ((double)H_WIDTH * lt_deltime[2]);
							if(tandeltheta > 0){
								//confirmed flff
								deltheta = arctan(tandeltheta);
								delx = (2*H_HEIGHT*(double)lt_deltime[0]/lt_deltime[2] + (double)H_WIDTH/tandeltheta) * (double)sqrt(1000000/(double)(1+tandeltheta*tandeltheta))/1024000 - H_GRID;
								tft_prints(0,6,"tan %f", tandeltheta);
								tft_prints(0, 7, "flff %f", deltheta); 
								tft_prints(0, 8, "delx %f", delx);
							}else{
								//check ffrf
								tandeltheta = (double)H_HEIGHT * (2 * lt_deltime[0] - lt_deltime[2]) / ((double)H_WIDTH * lt_deltime[2]);
								if(tandeltheta > 0){
									//confirmed ffrf
									deltheta = arctan(tandeltheta);
									delx = ((double)H_WIDTH/tandeltheta - 2*H_HEIGHT*(double)lt_deltime[1]/lt_deltime[2]) * (double)sqrt(1000000/(double)(1+tandeltheta*tandeltheta))/1024000 - H_GRID;
									tft_prints(0,6,"tan %f", tandeltheta);
									tft_prints(0, 7, "ffrf %f", deltheta); 
									tft_prints(0, 8, "delx %f", delx);
								}else{
									//unrecognized case
								}	
							}
						}
						break;
					case PATTERN_FLBR:
						break;
					case PATTERN_FRLB:
						tandeltheta = (double)(H_HEIGHT*((s32)lt_deltime[1]-(s32)lt_deltime[0]))/(double)(H_WIDTH*((s32)lt_deltime[0]+(s32)lt_deltime[1]));
						deltheta = arctan(tandeltheta);
						delscaledtheta = atan((s32)(tandeltheta * 32768));
						ratio_discrepancy = ((double)lt_deltime[0]/(double)lt_deltime[2]) / (double)(H_HEIGHT - H_WIDTH * tandeltheta)/(double)(2 * H_HEIGHT);
						if(LT_LOWERTOLERANCE < ratio_discrepancy && ratio_discrepancy < LT_UPPERTOLERANCE){
							tft_prints(0,6,"tan %f", tandeltheta);
							tft_prints(0,7, "ffff %f", deltheta); //confirm ffff
						}else{
							//check frff
							tandeltheta = (double)H_HEIGHT * (2 * lt_deltime[1] - lt_deltime[2]) / ((double)H_WIDTH * lt_deltime[2]);
							if(tandeltheta < 0){
								//confirmed frff
								deltheta = arctan(tandeltheta);
								delx = (2*H_HEIGHT*(double)lt_deltime[0]/lt_deltime[2] - (double)H_WIDTH/tandeltheta) * (double)sqrt(1000000/(double)(1+tandeltheta*tandeltheta))/1024000 + H_GRID;
								tft_prints(0,6,"tan %f", tandeltheta);
								tft_prints(0, 7, "flff %f", deltheta); 
								tft_prints(0, 8, "delx %f", delx);
							}else{
								//check fflf
								tandeltheta = (double)H_HEIGHT * (lt_deltime[2] - 2 * lt_deltime[0]) / ((double)H_WIDTH * lt_deltime[2]);
								if(tandeltheta > 0){
									//confirmed fflf
									deltheta = arctan(tandeltheta);
									delx = ((double)H_WIDTH/tandeltheta + 2*H_HEIGHT*(double)lt_deltime[1]/lt_deltime[2]) * (double)sqrt(1000000/(double)(1+tandeltheta*tandeltheta))/1024000 - H_GRID;
									tft_prints(0,6,"tan %f", tandeltheta);
									tft_prints(0, 7, "ffrf %f", deltheta); 
									tft_prints(0, 8, "delx %f", delx);
								}else{
									//unrecognized case
								}
							}
						}
						break;
					case PATTERN_FRBL:
						break;
					case PATTERN_FBLR:
						break;
					case PATTERN_FBRL:
						break;
					case PATTERN_LFRB:
						//check ffff
						tandeltheta = (double)(H_HEIGHT + (s32)lt_deltime[0]) / (double)(-H_WIDTH);
						ratio_discrepancy = ((double)lt_deltime[0]/(double)lt_deltime[1]) / ((double)(H_HEIGHT + H_WIDTH * tandeltheta)/(double)(2 * H_WIDTH * tandeltheta));
						tft_prints(7, 4, "%f", ratio_discrepancy); 	
						if(LT_LOWERTOLERANCE < ratio_discrepancy && ratio_discrepancy < LT_UPPERTOLERANCE){
							//confirmed ffff
							deltheta = arctan(tandeltheta);
							tft_prints(0,6,"tan %f", tandeltheta);
							tft_prints(0, 7, "ffff %f", deltheta); 
						}else{
							//check ffrf
							//assumed ffrf
							deltheta = arctan(tandeltheta);
							delx = ((double)lt_deltime[1] - (double)H_WIDTH/tandeltheta) * (double)sqrt(1000000/(double)(1+tandeltheta*tandeltheta))/1024000 + H_GRID;
								tft_prints(0,6,"tan %f", tandeltheta);
								tft_prints(0, 7, "ffrf %f", deltheta); 
								tft_prints(0, 8, "delx %f", delx);
						}
						break;
					case PATTERN_LFBR:
						break;
					case PATTERN_LRFB:
						break;
					case PATTERN_LRBF:
						break;
					case PATTERN_LBFR:
						break;
					case PATTERN_LBRF:
						break;
					case PATTERN_RFLB:
						//check ffff
						tandeltheta = (double)(H_HEIGHT + (s32)lt_deltime[0]) / (double)(H_WIDTH);
						ratio_discrepancy = ((double)lt_deltime[0]/(double)lt_deltime[1]) / ((double)(-H_HEIGHT + H_WIDTH * tandeltheta)/(double)(2 * H_WIDTH * tandeltheta));
						tft_prints(7, 4, "%f", ratio_discrepancy); 	
						if(LT_LOWERTOLERANCE < ratio_discrepancy && ratio_discrepancy < LT_UPPERTOLERANCE){
							//confirmed ffff
							deltheta = arctan(tandeltheta);
							tft_prints(0, 6,"tan %f", tandeltheta);
							tft_prints(0, 7, "ffff %f", deltheta); 
						}else{
							//check fflf
							//assumed fflf
							deltheta = arctan(tandeltheta);
							delx = ((double)lt_deltime[1] + (double)H_WIDTH/tandeltheta) * (double)sqrt(1000000/(double)(1+tandeltheta*tandeltheta))/1024000 - H_GRID;
								tft_prints(0, 6,"tan %f", tandeltheta);
								tft_prints(0, 7, "ffrf %f", deltheta); 
								tft_prints(0, 8, "delx %f", delx);
						}
						break;
					case PATTERN_RFBL:
						break;
					case PATTERN_RLFB:
						break;
					case PATTERN_RLBF:
						break;
					case PATTERN_RBFL:
						break;
					case PATTERN_RBLF:
						break;
					case PATTERN_BFLR:
						break;
					case PATTERN_BFRL:
						break;
					case PATTERN_BLFR:
						break;
					case PATTERN_BLRF:
						break;
					case PATTERN_BRFL:
						break;
					case PATTERN_BRLF:
						;
				}	
			}

			//Display visual white/black
			for(u8 i=8; i<16; ++i){
				for(u8 j=0; j<8; ++j)
					if(lt_value[DIR_F]) tft_put_pixel(i, j, WHITE);
					else tft_put_pixel(i, j, BLACK);
				for(u8 j=16; j<24; ++j)
					if(lt_value[DIR_B]) tft_put_pixel(i, j, WHITE);
					else tft_put_pixel(i, j, BLACK);
			}
			for(u8 j=8; j<16; ++j){
				for(u8 i=0; i<8; ++i)
					if(lt_value[DIR_L]) tft_put_pixel(i, j, WHITE);
					else tft_put_pixel(i, j, BLACK);
				for(u8 i=16; i<24; ++i)
					if(lt_value[DIR_R]) tft_put_pixel(i, j, WHITE);
					else tft_put_pixel(i, j, BLACK);
			}
			
			tft_update();
    }

	}

}
