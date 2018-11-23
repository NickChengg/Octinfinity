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
#define LT_F GPIO5
#define LT_L GPIO6
#define LT_R GPIO8
#define LT_B GPIO7

#define H_HEIGHT 20 //mm	//half height
#define H_WIDTH 40 //mm		//half width
#define H_GRID 467.5 //mm	//half grid width

#define LT_UPPERTOLERANCE 1.3
#define LT_LOWERTOLERANCE 0.7

#define PI 3.14159265

//Headers
#include "main.h"

#include "rcc.h"
#include "ticks.h"
#include "gpio.h"
#include "lcd_main.h"

#include <math.h>

typedef enum {
	DIR_F,
	DIR_L,
	DIR_R,
	DIR_B
} LT_DIRECTION;

typedef enum {
	PATTERN_FLRB = 0b00100100, PATTERN_FLBR = 0b00110100, PATTERN_FRLB = 0b00011000,
	PATTERN_FRBL = 0b00111000, PATTERN_FBLR = 0b00011100, PATTERN_FBRL = 0b00101100,
	PATTERN_LFRB = 0b00100001, PATTERN_LFBR = 0b00110001, PATTERN_LRFB = 0b00001001,
	PATTERN_LRBF = 0b00111001, PATTERN_LBFR = 0b00001101, PATTERN_LBRF = 0b00101101,
	PATTERN_RFLB = 0b00010010, PATTERN_RFBL = 0b00110010, PATTERN_RLFB = 0b00000110,
	PATTERN_RLBF = 0b00110110, PATTERN_RBFL = 0b00001110, PATTERN_RBLF = 0b00011110,
	PATTERN_BFLR = 0b00010011, PATTERN_BFRL = 0b00100011, PATTERN_BLFR = 0b00000111,
	PATTERN_BLRF = 0b00100111, PATTERN_BRFL = 0b00001011, PATTERN_BRLF = 0b00011011
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
  // Hardware Initialization
  rcc_init();																				//rcc
  ticks_init();																			//ticks
	gpio_init(LT_F, GPIO_Mode_IPU);										//gpio
	gpio_init(LT_L, GPIO_Mode_IPU);
	gpio_init(LT_R, GPIO_Mode_IPD);
	gpio_init(LT_B, GPIO_Mode_IPD);
	tft_init(PIN_ON_TOP, BLUE, WHITE, YELLOW, GREEN);	//tft
	tft_clear();
	
	// Variables Initialization
	u32 this_ticks = 0;
	u32 last_ticks_5 = 0;

	u8 lt_value[4], lt_last_value[4] = {0};
	u8 lt_pattern=0;
	u32 lt_basetime, lt_deltime[3];										//"del" = delta
	u8 lt_stage = 0;
	double tandeltheta, deltheta, delx;
	s32 delscaledtheta;
	double ratio_discrepancy; //temp
	
	char translate_dir[4] = {'F', 'L', 'R', 'B'};
	
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
					if(lt_stage<3) lt_pattern += i<<(2*i);
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
			
			//Determine pattern, calculate deviation
				//TODO Fully Categorize cases
			if(lt_stage==4){
				switch(lt_pattern){
					case PATTERN_FLRB:
						//check ffff
						tandeltheta = (double)(H_HEIGHT*((s32)lt_deltime[0]-(s32)lt_deltime[1]))/(double)(H_WIDTH*((s32)lt_deltime[0]+(s32)lt_deltime[1]));
						ratio_discrepancy = ((double)lt_deltime[0]/(double)lt_deltime[2]) / (double)(H_HEIGHT + H_WIDTH * tandeltheta)/(double)(2 * H_HEIGHT);
						if(LT_LOWERTOLERANCE < ratio_discrepancy && ratio_discrepancy < LT_UPPERTOLERANCE){
							//confirmed ffff
							deltheta = arctan(tandeltheta);
							delscaledtheta = atan((s32)(tandeltheta * 32768));
							tft_prints(0, 6, "Rot %f", deltheta); 
							//motor_adjust(0, deltheta);
						}else{
							//check flff
							tandeltheta = (double)H_HEIGHT * (lt_deltime[2] - 2 * lt_deltime[1]) / ((double)H_WIDTH * lt_deltime[2]);
							if(tandeltheta > 0){
								//confirmed flff
								deltheta = arctan(tandeltheta);
								delscaledtheta = atan((s32)(tandeltheta * 32768));
								delx = (2*H_HEIGHT*(double)lt_deltime[0]/lt_deltime[2] + (double)H_WIDTH/tandeltheta) * (double)sqrt(1000000/(double)(1+tandeltheta*tandeltheta))/1024000 - H_GRID;
								tft_prints(0, 6, "Rot %f", deltheta); 
							}else{
								//check ffrf
								tandeltheta = (double)H_HEIGHT * (2 * lt_deltime[0] - lt_deltime[2]) / ((double)H_WIDTH * lt_deltime[2]);
								if(tandeltheta > 0){
									//confirmed ffrf
									deltheta = arctan(tandeltheta);
									delscaledtheta = atan((s32)(tandeltheta * 32768));
									delx = ((double)H_WIDTH/tandeltheta - 2*H_HEIGHT*(double)lt_deltime[1]/lt_deltime[2]) * (double)sqrt(1000000/(double)(1+tandeltheta*tandeltheta))/1024000 - H_GRID;
									tft_prints(0, 6, "Rot %f", deltheta); 
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
						if(LT_LOWERTOLERANCE < ratio_discrepancy && ratio_discrepancy < LT_UPPERTOLERANCE)
							tft_prints(0, 6, "Rot %f", deltheta); //confirm ffff
							//motor_adjust(0, deltheta);
						else ;//flff OR ffrf
						break;
					case PATTERN_FRBL:
						break;
					case PATTERN_FBLR:
						break;
					case PATTERN_FBRL:
						break;
					case PATTERN_LFRB:
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
