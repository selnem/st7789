#include <stdio.h>
#include <bcm2835.h>
#include "st7789.h"
#include "images.h"
#include "screen.h"

void moveDot(int *x, int *y);
static int inRange(int x, int y);

int main(int argc, char **argv) {
    if (!bcm2835_init()) {
        printf("bcm2835_init failed. Are you running as root?\n");
        return 1;
    }
    
    bcm2835_gpio_fsel(TFT_DC, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(TFT_RST, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(26, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_set(26);

    
    st7789_init();
    while (1) {

    }
    
    bcm2835_spi_end();
    bcm2835_close();
    return 0;
}

static int inRange(int x, int y) {
    // keep 1-pixel margin for 3x3 dot
    if (x < 1 || x > 238 || y < 1 || y > 238) {
        return 0;
    }
    return 1;
}