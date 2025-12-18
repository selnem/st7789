#include <stdio.h>
#include <bcm2835.h>
#include "st7789.h"
#include "game.h"

int main(int argc, char **argv) {
    if (!bcm2835_init()) {
        printf("bcm2835_init failed");
        return 1;
    }
    
    bcm2835_gpio_fsel(TFT_DC, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(TFT_RST, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(26, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_set(26);

    st7789_init();
    st7789_joystick_init();
    st7789_buttons_init();  // 버튼 초기화
    
    // 게임 초기화
    reset_game();
    
    while (1) {
        play_game();
    }
    
    bcm2835_spi_end();
    bcm2835_close();
    return 0;
}