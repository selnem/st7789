#include<stdio.h>
#include "images.h"
#include "screen.h"
#include "game.h"
#include "hitbox.h"
#include "st7789.h"
#define MAXSPEED 20


static int MapSlideIdx=0;
static int PipesSlideIdx=-180;
static int slideSpeed=5;
const int acceleration=2;

void reset_game() {
    MapSlideIdx=0;
    PipesSlideIdx=-180;
    slideSpeed=5;
}

void map_slide() {
    MapSlideIdx += slideSpeed;

    const int w = bgMap_bitmap.width;
    MapSlideIdx%=w;

    set_map(&bgMap_bitmap, MapSlideIdx);
}

void pipes_slide() {
    PipesSlideIdx += slideSpeed;

    const int w = pipes_bitmap.width;
    if (w > 0) {
        PipesSlideIdx %= w;
    }

    set_map_pipes(&pipes_bitmap, PipesSlideIdx);
}
void update_acceleration() {
    static int cnt=0;
    cnt++;
    if (!(cnt%=20)&&slideSpeed<=MAXSPEED) {
        slideSpeed+=acceleration;
    }
}


void update_hitbox_screen(int airplane_x, int airplane_y) {

    reset_hitbox_screen();
    set_hitbox_pipes(&pipes_bitmap, PipesSlideIdx);
    
 
}