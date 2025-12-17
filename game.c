#include<stdio.h>
#include "images.h"
#include "screen.h"
#include "game.h"
#include "hitbox.h"
#include "st7789.h"
#define MAXSPEED 20


static int MapSlideIdx=0;
static int PipesSlideIdx=-180;
static int slideSpeed=1;
const int acceleration=2;

void map_slide() {
    MapSlideIdx += slideSpeed;

    int w = bgMap_bitmap.width;
    if (w > 0) {
    }

    set_map(&bgMap_bitmap, MapSlideIdx);
}

void pipes_slide() {
    PipesSlideIdx += slideSpeed;

    int w = pipes_bitmap.width;
    if (w > 0) {
        PipesSlideIdx %= w;
    }

    int y_offset = 0;
    set_map_pipes(&pipes_bitmap, PipesSlideIdx, y_offset);
}

void update_hitbox_screen(int airplane_x, int airplane_y) {

    reset_hitbox_screen();
    
    int y_offset = 0;
    set_hitbox_pipes(&pipes_bitmap, PipesSlideIdx, y_offset);
    
 
}