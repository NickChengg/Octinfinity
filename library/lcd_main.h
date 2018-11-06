#ifndef __LCD_MAIN_H
#define __LCD_MAIN_H

// 3.5inch
// #define MODEL_ILI9486
// 2.8/3.2inch
// #define MODEL_ILI9341
// 1.8inch
#define MODEL_ST7735

#include "gpio.h"
#include "ticks.h"
#include "spi.h"

typedef enum {
	PIN_ON_TOP,
	PIN_ON_LEFT,
	PIN_ON_BOTTOM,
	PIN_ON_RIGHT
}TFT_ORIENTATION;

/* GPIO settings */
#define TFT_RST &PB12
#define TFT_DC  &PB14
#define TFT_SPI_IRQ SPI2_IRQn
#define TFT_SPI_IRQH SPI2_IRQHandler

/* Colors */

#define	RGB888TO565(RGB888)  (((RGB888 >> 8) & 0xF800) |((RGB888 >> 5) & 0x07E0) | ((RGB888 >> 3) & 0x001F))

#define WHITE				(RGB888TO565(0xFFFFFF))
#define BLACK				(RGB888TO565(0x000000))
#define DARK_GREY		(RGB888TO565(0x555555))
#define GREY				(RGB888TO565(0xAAAAAA))
#define RED					(RGB888TO565(0xFF0000))
#define DARK_RED		(RGB888TO565(0x800000))
#define ORANGE			(RGB888TO565(0xFF9900))
#define YELLOW			(RGB888TO565(0xFFFF00))
#define GREEN				(RGB888TO565(0x00FF00))
#define DARK_GREEN	(RGB888TO565(0x00CC00))
#define BLUE				(RGB888TO565(0x0000FF))
#define BLUE2				(RGB888TO565(0x202060))
#define SKY_BLUE		(RGB888TO565(0x11CFFF))
#define CYAN				(RGB888TO565(0x8888FF))
#define PURPLE			(RGB888TO565(0x00AAAA))
#define PINK				(RGB888TO565(0xC71585))

/* Pixels */
#define CHAR_WIDTH              8
#define CHAR_HEIGHT             16

#ifdef MODEL_ILI9486
	
	#define MAX_WIDTH               320
	#define MAX_HEIGHT              480

	// #define CHAR_BUF_SIZE           600

	#define CHAR_MAX_X_VERTICAL     40
	#define CHAR_MAX_Y_VERTICAL     30

	#define CHAR_MAX_X_HORIZONTAL   60
	#define CHAR_MAX_Y_HORIZONTAL   20

	#define CHAR_MAX_X              60      // max between CHAR_MAX_X_VERTICAL and CHAR_MAX_X_HORIZONTAL
	#define CHAR_MAX_Y              30      // max between CHAR_MAX_Y_VERTICAL and CHAR_MAX_Y_HORIZONTAL

#endif

#ifdef MODEL_ILI9341

	#define MAX_WIDTH               240
	#define MAX_HEIGHT              320

	// #define CHAR_BUF_SIZE           600

	#define CHAR_MAX_X_VERTICAL     30
	#define CHAR_MAX_Y_VERTICAL     20

	#define CHAR_MAX_X_HORIZONTAL   40
	#define CHAR_MAX_Y_HORIZONTAL   15

	#define CHAR_MAX_X              40      // max between CHAR_MAX_X_VERTICAL and CHAR_MAX_X_HORIZONTAL
	#define CHAR_MAX_Y              20      // max between CHAR_MAX_Y_VERTICAL and CHAR_MAX_Y_HORIZONTAL

#endif

#ifdef MODEL_ST7735

	#define MAX_WIDTH               128
	#define MAX_HEIGHT              160

	// #define CHAR_BUF_SIZE           600

	#define CHAR_MAX_X_VERTICAL     16
	#define CHAR_MAX_Y_VERTICAL     10

	#define CHAR_MAX_X_HORIZONTAL   20
	#define CHAR_MAX_Y_HORIZONTAL   8

	#define CHAR_MAX_X              20      // max between CHAR_MAX_X_VERTICAL and CHAR_MAX_X_HORIZONTAL
	#define CHAR_MAX_Y              10      // max between CHAR_MAX_Y_VERTICAL and CHAR_MAX_Y_HORIZONTAL

#endif

extern u8 tft_orientation;
extern u16 curr_bg_color;
extern u16 curr_text_color;
extern u16 curr_text_color_sp;
extern u16 curr_highlight_color;

extern char text_buf[2][CHAR_MAX_X][CHAR_MAX_Y];
extern u16 text_color_buf[2][CHAR_MAX_X][CHAR_MAX_Y];
extern u16 bg_color_buf[2][CHAR_MAX_X][CHAR_MAX_Y];

// garuda
extern u8 char_max_x, char_max_y;

/* TFT communication control */
void tft_init(TFT_ORIENTATION orientation, u16 in_bg_color, u16 in_text_color, u16 in_text_color_sp, u16 in_highlight_color); //, DISPLAY_MODE mode);
void tft_enable(void);
void tft_disable(void);

/* Display properties configuration */
void tft_set_bg_color(u16 in_bg_color);
void tft_set_text_color(u16 in_text_color);
void tft_set_highlight_color(u16 sp_bg_color);
void tft_set_special_color(u16 text_color_sp);
u8 tft_get_orientation(void);

/* Pixel printing */
void tft_put_pixel(u16 x, u16 y, u16 color);
void tft_fill_color(u16 color);
void tft_force_clear(void);
void tft_set_region(u16 x, u16 y, u16 w, u16 h);
void tft_write_color(u16 color);

/* Text buffer printing */
void tft_clear_line(u8 line);
void tft_clear(void);
char tft_get_buffer_char(u8 x, u8 y);
u16 tft_get_buffer_color(u8 x, u8 y);
void tft_prints(u8 x, u8 y, const char * pstr, ...);
void tft_println(const char * pstr, ...);
void tft_update(void);

#endif		/* __LCD_MAIN_H */
