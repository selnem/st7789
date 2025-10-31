#include <stdio.h>
#include <bcm2835.h>
#include "st7789.h"

void moveDot(int *x, int *y);
int inRange(int x, int y);

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
    int x=120,y=120; 
    // Fill screen with different colors
    st7789_fillScreen(0xF800); // Red
    delay(1000);
    st7789_fillScreen(0x07E0); // Green
    delay(1000);
    st7789_fillScreen(0x001F); // Blue
    delay(1000);
    st7789_fillScreen(0xFFFF);
    while(1){
        st7789_drawDot(x, y, 0x0000);
        moveDot(&x, &y);
        st7789_drawDot(x,y,0xFFFF);
    }
    
    bcm2835_spi_end();
    bcm2835_close();
    return 0;
}
void moveDot(int *x, int *y) {
    JoystickState s = st7789_readJoystick();
    if (s.up) {
        if (inRange(*x, *y - 1)) {
            *y -= 1;
        }
    }
    if (s.down) {
        if (inRange(*x, *y + 1)) {
            *y += 1;
        }
    }
    if (s.left) {
        if (inRange(*x - 1, *y)) {
            *x -= 1;
        }
    }
    if (s.right) {
        if (inRange(*x + 1, *y)) {
            *x += 1;
        }
    }
}
int inRange(int x, int y) {
    // keep 1-pixel margin for 3x3 dot
    if (x < 1 || x > 238 || y < 1 || y > 238) {
        return 0;
    }
    return 1;
}