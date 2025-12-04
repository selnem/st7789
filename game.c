#include<stdio.h>
#include "images.h"
#include "screen.h"
#include "game.h"
#include "hitbox.h"
#define MAXSPEED 20


static int MapSlideIdx=0;
static int PipesSlideIdx=-180;  // 초기값: 화면 오른쪽에서 180 픽셀 여백
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
    
    // 파이프가 -180에서 시작해서 0이 된 이후로는 계속 진행
    // 파이프 맵의 너비를 넘으면 다시 -180으로 순환
    if (PipesSlideIdx >= pipes_bitmap.width) {
        PipesSlideIdx = -180;  // 다시 오른쪽에서 180 픽셀 여백 위치로 리셋
    }
    

    int y_offset = 0;
    
    set_map_pipes(&pipes_bitmap, PipesSlideIdx, y_offset);
}

void reset_map_slide() {
    MapSlideIdx = 0;
}

void reset_pipes_slide() {
    PipesSlideIdx = -180;  // 초기 위치: 화면 오른쪽에서 180 픽셀 여백
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