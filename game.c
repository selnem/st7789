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
    
   
    if (MapSlideIdx >= bgMap_bitmap.width) {
        MapSlideIdx = 0;
    }
    
    set_map(&bgMap_bitmap, MapSlideIdx);
}

void pipes_slide() {
    
    PipesSlideIdx += slideSpeed;

    if (PipesSlideIdx >= pipes_bitmap.width) {
        PipesSlideIdx = -180;
    }
    

    int y_offset = 0;
    
    set_map_pipes(&pipes_bitmap, PipesSlideIdx, y_offset);
}

void reset_map_slide() {
    MapSlideIdx = 0;
}

void reset_pipes_slide() {
    PipesSlideIdx = -180;
}

int get_map_slide_idx() {
    return MapSlideIdx;
}

int get_pipes_slide_idx() {
    return PipesSlideIdx;
}

void set_slide_speed(int speed) {
    if (speed >= 0 && speed <= MAXSPEED) {
        slideSpeed = speed;
    }
}

void update_hitbox_screen(int airplane_x, int airplane_y) {

    reset_hitbox_screen();
    
    int y_offset = 0;
    set_hitbox_pipes(&pipes_bitmap, PipesSlideIdx, y_offset);
    
 
}