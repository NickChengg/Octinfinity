#ifndef __SSD1306_H
#define __SSD1306_H

#define OLED_SPI (TFT_SPI.periph)
#define TFT_RST &PB12
#define TFT_DC  &PB14
#include "spi.h"
#include "gpio.h"
#include "ticks.h"

typedef enum {
    OLEDScrollRight = 0x26, OLEDScrollLeft = 0x27, OLEDScrollDiagRight = 0x29, OLEDScrollDiagLeft = 0x2A
} OLEDScrollDirection;

typedef enum {
    OLEDScroll5Frames,
    OLEDScroll64Frames,
    OLEDScroll128Frames,
    OLEDScroll256Frames,
    OLEDScroll3Frames,
    OLEDScroll4Frames,
    OLEDScroll25Frames,
    OLEDScroll2Frames,
} OLEDScrollSpeed;

void oled_init(void);

void oled_clear(void);

void oled_flash(void);

void oled_write_char(uint8_t x, uint8_t y, uint8_t ch);

void oled_write_str(uint8_t x, uint8_t y, uint8_t wrapx, const char *pstr, ...);
void oled_print_str(uint8_t x, uint8_t y, uint8_t wrapx, const char *pstr, ...);

void oled_write_image(uint8_t x, uint8_t y, uint8_t w, uint8_t *lines, uint16_t len);

void oled_update(void);

void oled_scroll(OLEDScrollDirection dir, uint8_t start, uint8_t stop, OLEDScrollSpeed spd, uint8_t vertamt, uint8_t vfixed,
                 uint8_t vscroll);

void oled_stop_scroll(void);

#endif
