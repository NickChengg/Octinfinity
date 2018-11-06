cmake_minimum_required(VERSION 3.10)
project(library C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(library
        adc.c
        adc.h
        buttons.c
        buttons.h
        buzzer.c
        buzzer.h
        camera.c
        camera.h
        gpio.c
        gpio.h
        i2c.c
        i2c.h
        lcd_font.h
        lcd_init.h
        lcd_main.c
        lcd_main.h
        leds.c
        leds.h
        math.c
        math.h
        ov7725.h
        ppi.c
        ppi.h
        pwm.c
        pwm.h
        rcc.c
        rcc.h
        spi.c
        spi.h
        ticks.c
        ticks.h
        uart.c
        uart.h)
