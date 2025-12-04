#include <bcm2835.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "st7789.h"


void writeCommand(uint8_t cmd) {
    bcm2835_gpio_clr(TFT_DC);
    bcm2835_spi_transfer(cmd);
}

void writeData(uint8_t data) {
    bcm2835_gpio_set(TFT_DC);
    bcm2835_spi_transfer(data);
}


void st7789_init() {
    bcm2835_spi_begin();
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_8);
    
    // Hardware reset
    bcm2835_gpio_clr(TFT_RST);
    delay(100);
    bcm2835_gpio_set(TFT_RST);
    delay(100);

    writeCommand(ST7789_SWRESET); // Software reset
    delay(150);

    writeCommand(ST7789_SLPOUT);  // Sleep out
    delay(500);

    writeCommand(ST7789_COLMOD);  // Set color mode
    writeData(0x55);              // 16-bit color
    delay(10);

    writeCommand(ST7789_MADCTL);
    writeData(0x00);              // Normal display
    writeCommand(0x21);
    writeCommand(ST7789_CASET);
    writeData(0x00); writeData(0x00); // XSTART = 0
    writeData(ST7789_TFTWIDTH >> 8); writeData(ST7789_TFTWIDTH & 0xFF); // XEND

    writeCommand(ST7789_RASET);
    writeData(0x00); writeData(0x00); // YSTART = 0
    writeData(ST7789_TFTHEIGHT >> 8); writeData(ST7789_TFTHEIGHT & 0xFF); // YEND

    writeCommand(ST7789_DISPON);  // Display on
    delay(100);
}

void st7789_fillScreen(uint16_t color) {
    uint8_t hi = color >> 8, lo = color & 0xFF;
    writeCommand(ST7789_RASET);
    writeData(0); writeData(0);
    writeData(ST7789_TFTHEIGHT >> 8); writeData(ST7789_TFTHEIGHT & 0xFF);
    
    writeCommand(ST7789_CASET);
    writeData(0); writeData(0);
    writeData(ST7789_TFTWIDTH >> 8); writeData(ST7789_TFTWIDTH & 0xFF);
    
    writeCommand(ST7789_RAMWR);
    bcm2835_gpio_set(TFT_DC);

    int i, j;
    for (i = 0; i < ST7789_TFTHEIGHT; i++) {
        for (j = 0; j < ST7789_TFTWIDTH; j++) {
            bcm2835_spi_transfer(hi);
            bcm2835_spi_transfer(lo);
        }
    }
}

void st7789_drawDot(int x, int y, uint16_t color) {
    // Clamp drawing window to valid bounds (3x3 dot centered at x,y)
    int x0 = x - 1;
    int x1 = x + 1;
    int y0 = y - 1;
    int y1 = y + 1;
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 >= ST7789_TFTWIDTH) x1 = ST7789_TFTWIDTH - 1;
    if (y1 >= ST7789_TFTHEIGHT) y1 = ST7789_TFTHEIGHT - 1;

    uint8_t hi = color >> 8, lo = color & 0xFF;

    // Set column address
    writeCommand(ST7789_CASET);
    writeData((x0 >> 8) & 0xFF); writeData(x0 & 0xFF);
    writeData((x1 >> 8) & 0xFF); writeData(x1 & 0xFF);

    // Set row address
    writeCommand(ST7789_RASET);
    writeData((y0 >> 8) & 0xFF); writeData(y0 & 0xFF);
    writeData((y1 >> 8) & 0xFF); writeData(y1 & 0xFF);

    // Write pixel data (3x3 = up to 9 pixels)
    writeCommand(ST7789_RAMWR);
    bcm2835_gpio_set(TFT_DC);
    int width = (x1 - x0 + 1);
    int height = (y1 - y0 + 1);
    int count = width * height;
    for (int i = 0; i < count; i++) {
        bcm2835_spi_transfer(hi);
        bcm2835_spi_transfer(lo);
    }
}


void st7789_joystick_init() {
    bcm2835_gpio_fsel(JOYSTICK_UP, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(JOYSTICK_UP, BCM2835_GPIO_PUD_UP);
    bcm2835_gpio_fsel(JOYSTICK_DOWN, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(JOYSTICK_DOWN, BCM2835_GPIO_PUD_UP);
    bcm2835_gpio_fsel(JOYSTICK_LEFT, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(JOYSTICK_LEFT, BCM2835_GPIO_PUD_UP);
    bcm2835_gpio_fsel(JOYSTICK_RIGHT, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(JOYSTICK_RIGHT, BCM2835_GPIO_PUD_UP);
}

JoystickState st7789_readJoystick() {
    JoystickState s;
    s.up = (bcm2835_gpio_lev(JOYSTICK_UP) == 0) ? 1 : 0;
    s.down = (bcm2835_gpio_lev(JOYSTICK_DOWN) == 0) ? 1 : 0;
    s.left = (bcm2835_gpio_lev(JOYSTICK_LEFT) == 0) ? 1 : 0;
    s.right = (bcm2835_gpio_lev(JOYSTICK_RIGHT) == 0) ? 1 : 0;
    return s;
}

void st7789_writeCmds() {
    writeCommand(ST7789_RASET);
    writeData(0); writeData(0);
    writeData((ST7789_TFTHEIGHT >> 8) & 0xFF); writeData(ST7789_TFTHEIGHT & 0xFF);

    writeCommand(ST7789_CASET);
    writeData(0); writeData(0);
    writeData((ST7789_TFTWIDTH >> 8) & 0xFF); writeData(ST7789_TFTWIDTH & 0xFF);

    writeCommand(ST7789_RAMWR);
    bcm2835_gpio_set(TFT_DC);
}


#define DEBOUNCE_DELAY_MS 50


static uint8_t last_button_a_state = 1;
static uint8_t last_button_b_state = 1;
static uint32_t last_button_a_change_time = 0;
static uint32_t last_button_b_change_time = 0;

void st7789_buttons_init() {
    bcm2835_gpio_fsel(BUTTON_A, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(BUTTON_A, BCM2835_GPIO_PUD_UP);
    
    bcm2835_gpio_fsel(BUTTON_B, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(BUTTON_B, BCM2835_GPIO_PUD_UP);

    delay(10);
}

ButtonState st7789_readButtons() {
    ButtonState state;
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    uint32_t current_time_ms = (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);

    uint8_t raw_a = (bcm2835_gpio_lev(BUTTON_A) == 0) ? 1 : 0;
    uint8_t raw_b = (bcm2835_gpio_lev(BUTTON_B) == 0) ? 1 : 0;

    if (raw_a != last_button_a_state) {

        last_button_a_change_time = current_time_ms;
        last_button_a_state = raw_a;
        state.a = 0;
    } else {
        if ((current_time_ms - last_button_a_change_time) >= DEBOUNCE_DELAY_MS) {
            state.a = raw_a;
        } else {
            state.a = 0;
        }
    }
    if (raw_b != last_button_b_state) {
        last_button_b_change_time = current_time_ms;
        last_button_b_state = raw_b;
        state.b = 0;
    } else {
        if ((current_time_ms - last_button_b_change_time) >= DEBOUNCE_DELAY_MS) {
            state.b = raw_b;
        } else {
            state.b = 0;
        }
    }
    
    return state;
}