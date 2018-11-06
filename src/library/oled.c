#include <stdarg.h>
#include "oled.h"

#define FONT_WIDTH 5
#define FONT_HEIGHT 8

#include "oled_font.h"

#define RES_128X64
#ifdef RES_128X64
#define WIDTH 128
#define HEIGHT 64
#define PAGES 8
#define ROWS PAGES
#if FONT_WIDTH == 3
#define COLS 42
#endif
#if FONT_WIDTH == 5
#define COLS 25
#endif
#if FONT_WIDTH == 6
#define COLS 21
#endif
#if FONT_WIDTH == 8
#define COLS 16
#endif
#endif

uint8_t pixel_buf[WIDTH][PAGES] = {0};

static void reset(void) {
    gpio_set(TFT_RST);
    delay(100);
    gpio_reset(TFT_RST);
    delay(100);
    gpio_set(TFT_RST);
}

static void write_command(uint8_t command) {
    gpio_reset(TFT_DC);

    while (!(OLED_SPI->SR & SPI_SR_TXE));
    OLED_SPI->DR = command;
}

static void write_data(uint8_t data) {
    gpio_set(TFT_DC);

    while (!(OLED_SPI->SR & SPI_SR_TXE));
    OLED_SPI->DR = data;
}

void oled_init(void) {
    spi_init(&TFT_SPI, SPI_BaudRatePrescaler_2, SPI_Direction_1Line_Tx, SPI_Mode_Master, SPI_DataSize_8b); //TODO: test SPI_BaudRatePrescaler_2

		gpio_init(TFT_RST, GPIO_Mode_Out_PP);
		gpio_init(TFT_DC, GPIO_Mode_Out_PP);

    reset();
    //gpio_reset(TFT_CS);

    write_command(0xAE); // Set display OFF

    write_command(0xD5); // Set Display Clock Divide Ratio / OSC Frequency
    write_command(0x80); // Display Clock Divide Ratio / OSC Frequency 

    write_command(0xA8); // Set Multiplex Ratio
    write_command(0x3F); // Multiplex Ratio for 128x64 (64-1)

    write_command(0xD3); // Set Display Offset
    write_command(0x00); // Display Offset

    write_command(0x40); // Set Display Start Line

    write_command(0x8D); // Set Charge Pump
    write_command(0x14); // Charge Pump (0x10 External, 0x14 Internal DC/DC)

    write_command(0xA1); // Set Segment Re-Map
    write_command(0xC8); // Set Com Output Scan Direction

    write_command(0xDA); // Set COM Hardware Configuration
    write_command(0x12); // COM Hardware Configuration

    write_command(0x81); // Set Contrast
    write_command(0xCF); // Contrast

    write_command(0xD9); // Set Pre-Charge Period
    write_command(0xF1); // Set Pre-Charge Period (0x22 External, 0xF1 Internal)

    write_command(0xDB); // Set VCOMH Deselect Level
    write_command(0x40); // VCOMH Deselect Level

    write_command(0x20); //Set memory addressing mode
    write_command(0x00); //Horizontal

//    write_command(0x2E);

    write_command(0xA4); // Set all pixels OFF
    write_command(0xA6); // Set display not inverted
    write_command(0xAF); // Set display On

    delay(100);
    oled_clear();

}

static inline void set_region(uint8_t sx, uint8_t ex, uint8_t sy) {
    write_command(0x21);
    write_command(sx);
    write_command(ex-1);

    write_command(0x22);
    write_command(sy);
    write_command(PAGES - 1);
//    write_command(HEIGHT);
}

static inline void set_cursor(uint8_t x, uint8_t y) {
    write_command(0x21);
    write_command(x);
    write_command(WIDTH);

    write_command(0x22);
    write_command(y);
    write_command(PAGES - 1);
//    write_command(HEIGHT);
}

void oled_clear(void) {
    set_region(0, WIDTH, 0);
//    delay(1);
    for (int i = WIDTH * PAGES; i; i--) {
        write_data(0x00);
    }
}

void oled_flash(void) {
    set_region(0, WIDTH, 0);
    for (int i = WIDTH * PAGES; i; i--) {
        write_data(0xFF);
    }
}

static inline void write_char(uint8_t ch) {
    const uint8_t *cptr = CHAR_PTR(ch);
    for (uint8_t i = FONT_WIDTH; i; i--) write_data(*cptr++);
}

void oled_write_char(uint8_t x, uint8_t y, uint8_t ch) {
    set_region(x, x + FONT_WIDTH, y);
    write_char(ch);
}

void oled_write_str(uint8_t x, uint8_t y, uint8_t wrapx, const char *pstr, ...) {
    set_region(x, x + wrapx * FONT_WIDTH, y);

    uint8_t buf[256];
    va_list arglist;
    va_start(arglist, pstr);
    vsprintf((char *) buf, (const char *) pstr, arglist);
    va_end(arglist);
    uint8_t * str = &buf[0];

    while (*str) {
        write_char(*str++);
    }
}

void oled_print_char(uint8_t x, uint8_t y, uint8_t ch) {
    const uint8_t *cptr = CHAR_PTR(ch);

    for (uint8_t i = 0; i < FONT_WIDTH; i++) {
        pixel_buf[x + i][y] = *(cptr + i);
    }
}

void oled_print_str(uint8_t x, uint8_t y, uint8_t wrapx, const char *pstr, ...) {
    uint8_t wx = x + wrapx * FONT_WIDTH, ox = x;

    uint8_t buf[256];
    va_list arglist;
    va_start(arglist, pstr);
    vsprintf((char *) buf, (const char *) pstr, arglist);
    va_end(arglist);
    uint8_t * str = &buf[0];

    while (*str) {
        uint8_t ch = *str++;
        const uint8_t *cptr = CHAR_PTR(ch);
        for (uint8_t i = FONT_WIDTH; i; i--) pixel_buf[x++][y] = *cptr++;
        if (x == wx) {
            x = ox;
            if (y++ == PAGES) y = 0;
        }
    }
}

void oled_update(void) {
    set_region(0, WIDTH, 0);

    //TODO: DMA Mode
    for (uint8_t i = 0; i<PAGES; i++) {
        for (uint8_t j = 0; j<WIDTH; j++) {
            write_data(pixel_buf[j][i]);
        }
    }
}

void oled_write_image(uint8_t x, uint8_t y, uint8_t w, uint8_t *lines, uint16_t len) {
    set_region(x, x + w, y);
    while (len--) {
        write_data(*lines++);
    }
}

void oled_scroll(OLEDScrollDirection dir, uint8_t start, uint8_t stop, OLEDScrollSpeed spd, uint8_t vertamt, uint8_t vfixed,
            uint8_t vscroll) {
    write_command(0x2E); //Disable scrolling
    if (dir & 0x8) {
        write_command(0xA3);
        write_command(vfixed);
        write_command(vscroll);
    }
    write_command(dir); //Setup scrolling
    write_command(0); //dummy
    write_command(start);
    write_command(spd); //time interval
    write_command(stop);
    if (dir & 0x4) {
        write_command(0); //dummy
        write_command(0xFF); //dummy
    } else {
        write_command(vertamt);
    }
    write_command(0x2F); //Enable scrolling
}

void oled_stop_scroll(void) {
    write_command(0x2E);
}
