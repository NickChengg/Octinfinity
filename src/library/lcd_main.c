#include "lcd_main.h"
#include "lcd_font.h"
#include "lcd_init.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

//private data
u16 curr_bg_color = BLACK;
u16 curr_highlight_color = BLACK;
u16 curr_text_color = BLACK;
u16 curr_text_color_sp = BLACK;

#ifdef MODEL_ST7735
static u8 col_shift;
static u8 row_shift;
#endif

u8 tft_orientation = 0, tft_enabled = 0;
u16 tft_y_index = 0;
u16 tft_x_index = 0;
u8 char_max_x, char_max_y;

u8 cur_screen, next_screen;
u8 pointer_to_curr_screen = 0;
char text_buf[2][CHAR_MAX_X][CHAR_MAX_Y];
u16 text_color_buf[2][CHAR_MAX_X][CHAR_MAX_Y];
u16 bg_color_buf[2][CHAR_MAX_X][CHAR_MAX_Y];

//u16 pixel_buf[MAX_WIDTH][MAX_HEIGHT] = {0};

/**
  * @brief  Initialization of SPI for TFT
  * @param  None
  * @retval None
  */
void tft_spi_init(void) {
	spi_init(&TFT_SPI, SPI_BaudRatePrescaler_2, SPI_Direction_1Line_Tx, SPI_Mode_Master, SPI_DataSize_8b); //TODO: test SPI_BaudRatePrescaler_2

	gpio_init(TFT_RST, GPIO_Mode_Out_PP);
	gpio_init(TFT_DC, GPIO_Mode_Out_PP);

	// spi_it_init(&TFT_SPI);
}

/**
  * @brief  Sending a command
  * @param  command: one byte command to be sent
  * @retval None
  */
static inline void tft_write_command(u8 command) {
	gpio_reset(TFT_DC);
	spi_tx(&TFT_SPI, command);
}

/**
  * @brief  Sending a data
  * @param  data: one byte data to be sent
  * @retval None
  */
static inline void tft_write_data(u8 data) {
	gpio_set(TFT_DC);
	spi_tx(&TFT_SPI, data);
}


/**
  * @brief  Configuration of TFT
  * @param  None
  * @retval None
  */
inline static void tft_config(void)
{
	u32 i=0;
	while (tft_init_code[++i] != TFT_EOF_MARKER) {
		tft_write_command(tft_init_code[i]);
		
		if (tft_init_code[i] == TFT_ORIENTATION_CMD) tft_write_data(tft_orientation_init_code[tft_orientation]);

		while (tft_init_code[++i] != TFT_EOL_MARKER) {
			if (tft_init_code[i] == TFT_DELAY_MARKER) delay(tft_init_code[++i]);
			else                                      tft_write_data(tft_init_code[i]);
		}
	}
}

/**
  * @brief  Hardware reset for TFT
  * @param  None
  * @retval None
  */
void tft_reset(void)
{
 	gpio_reset(TFT_RST);
	delay(100);
	gpio_set(TFT_RST);
	delay(100);
}

/**
  * @brief  Initialization of TFT
  * @param  orientation: default orientation
  * @param  in_bg_color: default background color
  * @param  in_text_color: default text color
  * @param  in_text_color_sp: default special text color
  * @retval None
  */
void tft_init(TFT_ORIENTATION orientation, u16 in_bg_color, u16 in_text_color, u16 in_text_color_sp, u16 in_highlight_color) {

	tft_y_index = 0;
	cur_screen = 0;
	next_screen = 1;
	tft_orientation = orientation;

	#ifdef MODEL_ST7735
	col_shift = (tft_orientation % 2) ? 1 : 2;
	row_shift = (tft_orientation % 2) ? 2 : 1;
	#endif

	tft_spi_init();
	
	//Hardware reset
	tft_reset();
	tft_config();
	
	tft_set_bg_color(in_bg_color);
	tft_set_text_color(in_text_color);
	tft_set_special_color(in_text_color_sp);
	tft_set_highlight_color(in_highlight_color);

	tft_fill_color(in_bg_color);
	
	char_max_x = (orientation % 2) ? CHAR_MAX_X_HORIZONTAL : CHAR_MAX_X_VERTICAL;
	char_max_y = (orientation % 2) ? CHAR_MAX_Y_HORIZONTAL : CHAR_MAX_Y_VERTICAL;

	tft_enable();

	tft_clear();
	tft_update();
}

/**
  * @brief  Enable using TFT
  * @param  None
  * @retval None
  */
void tft_enable(void) {
	tft_enabled = 1;
}

/**
  * @brief  Disable using TFT
  * @param  None
  * @retval None
  */
void tft_disable(void) {
	tft_enabled = 0;
}

/**
  * @brief  Set the current background color
  * @param  None
  * @retval None
  */
void tft_set_bg_color(u16 in_bg_color) {
	curr_bg_color = in_bg_color;
}

/**
  * @brief  Set the current special background color
  * @param  None
  * @retval None
  */
void tft_set_highlight_color(u16 highlight_color) {
	curr_highlight_color = highlight_color;
}

/**
  * @brief  Set the current text color
  * @param  None
  * @retval None
  */
void tft_set_text_color(u16 in_text_color) {
	curr_text_color = in_text_color;
}

/**
  * @brief  Set the current special text color
  * @param  None	
  * @retval None
  */
void tft_set_special_color(u16 text_color_sp) {
	curr_text_color_sp = text_color_sp;
}

u8 tft_get_orientation() {
	return tft_orientation;
}

void tft_set_region(u16 x, u16 y, u16 w, u16 h) {
	u16 startx = x;
	u16 starty = y;
	u16 endx = x+w;
	u16 endy = y+h;

	#ifdef MODEL_ST7735
	startx += col_shift;
	starty += row_shift;
	endx += col_shift;
	endy += row_shift;
	#endif

	tft_write_command(COL_ADDR_SET_COMMAND);		// Column addr set
	tft_write_data((startx>>8 & 0xFF));
	tft_write_data((startx)   & 0xFF); 				// X START
	tft_write_data((endx>>8   & 0xFF));
	tft_write_data((endx)     & 0xFF);

	tft_write_command(ROW_ADDR_SET_COMMAND);		// Row addr set
	tft_write_data((starty>>8 & 0xFF));
	tft_write_data((starty)   & 0xFF);				// Y START
	tft_write_data((endy>>8   & 0xFF));
	tft_write_data((endy)     & 0xFF);

	tft_write_command(WRITE_RAM_COMMAND);

	gpio_set(TFT_DC);
}

#define buf_coord(x, y) (((u16)(y)) * ((u16)char_max_x) + ((u16)(x)))

/**
  * @brief  Clear one line in the text buffer
  * @param  line: the line to be cleared
  * @retval None
  */
void tft_clear_line(u8 y){
	for (u8 x = 0; x < char_max_x; x++) {
		text_buf[cur_screen][x][y] = ' ';
		text_color_buf[cur_screen][x][y] = curr_text_color;
		bg_color_buf[cur_screen][x][y] = curr_bg_color;
	}
}

/**
  * @brief  Clear the whole text buffer
  * @param  None
  * @retval None
  */
void tft_clear(void){
	tft_y_index = 0;
	for(u8 y = 0; y < char_max_y; y++){
		tft_clear_line(y);
	}
}

void tft_write_color(u16 color) {
	#ifdef RGB666_MODE //RGB666
	tft_write_data((color & 0xF800) >> (11-3));
	tft_write_data((color & 0x7E0) >> (5-2));
	tft_write_data((color & 0x1F) << 3);
	#endif

	#ifdef RGB565_MODE
	tft_write_data(color >> 8);
	tft_write_data(color);
	#endif
}

/**
  * @brief  Print a single pixel on screen
  * @param  x: x-coordinate
  * @param  y: y-coordinate
  * @param  color: color of the pixel
  * @retval None
  */
void tft_put_pixel(u16 x, u16 y, u16 color) {
	tft_set_region(x, y, 1, 1);
	tft_write_color(color);
}

/**
  * @brief  Fill the whole screen with a color
  * @param  color: color to be filled with
  * @retval None
  */
void tft_fill_color(u16 color) {
	u32 i;
	if (tft_orientation % 2)
		tft_set_region(0, 0, MAX_HEIGHT, MAX_WIDTH);
	else
		tft_set_region(0, 0, MAX_WIDTH, MAX_HEIGHT);
	
	for (i = 0; i < MAX_WIDTH*MAX_HEIGHT; i++) {
		tft_write_color(color);
	}
}

/**
  * @brief  Clear every pixel on screen
  * @param  None
  * @retval None
  */
void tft_force_clear(void) {
	for (u8 i = 0; i < CHAR_MAX_X; i++) {
		for (u8 j = 0; j < CHAR_MAX_Y; j++) {
			text_buf[0][i][j] = ' ';
			text_buf[1][i][j] = ' ';
		}
	}
	tft_fill_color(curr_bg_color);
}

inline static void tft_incr_line() {
    tft_y_index++;
	if (tft_y_index >= char_max_y) tft_y_index = 0;
}

static void tft_set_buffer(u8 x, u8 y, u8 * fp) {
	u8 i = x;
	u8 j = y;
	u8 is_special = 0;
	u8 is_highlighted = 0;
	u8 is_underlined = 0;
	// u8 escaped = 0;
	
	while (*fp && i < char_max_x && j < char_max_y) {
		switch(*fp) {
			case '\r':
			case '\n':
				j++;
				i=0;
				tft_incr_line();
				break;
			case '[':
			case ']':
				is_special = (*fp == '[');
				break;
			case '{':
			case '}':
				is_highlighted = (*fp == '{');
				break;
			case '|':
				is_underlined = !is_underlined;
				break;
			default:
				text_buf[cur_screen][i][j] = ((*fp) | (is_underlined ? 0x80 : 0x00));
				text_color_buf[cur_screen][i][j] = is_special     ? curr_text_color_sp   : curr_text_color;
				bg_color_buf[cur_screen][i++][j] = is_highlighted ? curr_highlight_color : curr_bg_color;
				break;
		}
		fp++;
	}

	tft_x_index = i;
}

char tft_get_buffer_char(u8 x, u8 y) {
	return text_buf[cur_screen][x][y];
}

u16 tft_get_buffer_color(u8 x, u8 y) {
	return text_color_buf[cur_screen][x][y];
}

/**
  * @brief  Print a string at certain position, use [] to indicate special words
  * @param  x: starting x-coordinate
  * @param  y: starting y-coordinate
  * @param  pstr: string to be printed
  */
void tft_prints(u8 x, u8 y, const char * pstr, ...){
	if (x >= char_max_x || y >= char_max_y)
		return;
	
	u8 buf[256];
	va_list arglist;
	va_start(arglist, pstr);
	vsprintf((char*)buf, (const char*)pstr, arglist);
	va_end(arglist);
	
	tft_set_buffer(x, y, buf);
}

/**
	* Append a line to the tft screen. Not affected by tft_prints
	* @param pstr: Stuff to be printed
	*/
void tft_println(const char * pstr, ...){
	u8 buf[256];
	va_list arglist;
	va_start(arglist, pstr);
	vsprintf((char*)buf, (const char*)pstr, arglist);
	va_end(arglist);
	
	tft_set_buffer(tft_x_index, tft_y_index, buf);

	tft_incr_line();
	tft_x_index = 0;
}

void tft_print(const char * pstr, ...){
	u8 buf[256];
	va_list arglist;
	va_start(arglist, pstr);
	vsprintf((char*)buf, (const char*)pstr, arglist);
	va_end(arglist);
	
	tft_set_buffer(tft_x_index, tft_y_index, buf);
}

static inline bool tft_char_is_changed(u8 x, u8 y){
	return !((text_buf[cur_screen][x][y] == text_buf[next_screen][x][y])
						&& (text_color_buf[cur_screen][x][y] == text_color_buf[next_screen][x][y])
						&& (bg_color_buf[cur_screen][x][y] == bg_color_buf[next_screen][x][y]));
}

/**
  * @brief  Refresh the whole screen
  * @param  None
  * @retval None
  */
void tft_update(void) {
	u8 * char_ptr;
	s16 adj_cnt = 0;
	u16 color;
	u8 char_cnt = 0;
	u8 y = 0;
	
	if (!tft_enabled)
		return;

	for (u8 j = 0; j < char_max_y; j++) {
		for (u8 i = 0; i < char_max_x; i++) {
			if (tft_char_is_changed(i, j)) {
				adj_cnt = 1;
				while ((i + adj_cnt < char_max_x) && tft_char_is_changed(i + (adj_cnt), j)) adj_cnt++; //Search for diff char
				tft_set_region(i*CHAR_WIDTH, j*CHAR_HEIGHT, adj_cnt*CHAR_WIDTH-1, CHAR_HEIGHT-1);
				
				y = 0;
				while (y < CHAR_HEIGHT) {
					char_ptr = CHAR_PTR(text_buf[cur_screen][i + char_cnt][j]);
					for (u8 x = CHAR_WIDTH; x > 0; x--) {
						color = (char_ptr[y] >> (x - 1) & 0x01) ? text_color_buf[cur_screen][i + char_cnt][j] : bg_color_buf[cur_screen][i + char_cnt][j];
						tft_write_color(color);
					}
					if (++char_cnt == adj_cnt) {
						char_cnt = 0;
						y++;
					}
				}
				i += adj_cnt-1;
			}
		}
	}
	
	// Swap pointers
	cur_screen = (cur_screen == 0);
	next_screen = (next_screen == 0);
}
