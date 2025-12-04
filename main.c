#include <stdio.h>
#include <bcm2835.h>
#include <unistd.h>
#include "st7789.h"
#include "images.h"
#include "screen.h"
#include "game.h"
#include "physics.h"

// 비행기 위치 범위 체크 함수
int airplaneInRange(int x, int y) {
    // 화면 범위 내에 있는지 확인
    return (x >= 0 && x + airplane_bitmap.height <= screen_bitmap.height &&
            y >= 0 && y + airplane_bitmap.width <= screen_bitmap.width);
}

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
    st7789_joystick_init();
    
    // 비행기 초기 위치 (화면 중앙)
    int airplane_x = (screen_bitmap.height - airplane_bitmap.height) / 2;
    int airplane_y = 50;
    
    printf("Game started. Use joystick to move airplane.\n");
    
    while (1) {
        // 화면 초기화
        reset_screen();
        
        // 배경 스크롤
        map_slide();
        
        // 파이프 스크롤
        pipes_slide();
        
        // 조이스틱으로 비행기 이동
        moveObj(&airplane_x, &airplane_y, 2, airplaneInRange);
        
        // 비행기 그리기
        set_obj(airplane_x, airplane_y, &airplane_bitmap);
        
        // 화면에 출력
        draw_screen();
        
        // 프레임 딜레이 (약 60 FPS)
        usleep(1);  // 약 16.666ms (1/60초)
    }
    
    bcm2835_spi_end();
    bcm2835_close();
    return 0;
}