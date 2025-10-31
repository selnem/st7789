#include <bcm2835.h>
#include <stdio.h>

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
    uint8_t hi = color >> 8, lo = color & 0xFF;
    writeCommand(ST7789_CASET);
    writeData((x-1) >> 8); writeData((x-1) & 0xFF);
    writeData((x+1) >> 8); writeData((x+1) & 0xFF);
    writeCommand(ST7789_RASET);
    writeData((y-1) >> 8); writeData((y-1) & 0xFF);
    writeData((y+1) >> 8); writeData((y+1) & 0xFF);
    writeCommand(ST7789_RAMWR);
    bcm2835_gpio_set(TFT_DC);
    for(int i = 0; i < 9; i++) {
        bcm2835_spi_transfer(hi);
        bcm2835_spi_transfer(lo);
    }
}
