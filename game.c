#include<stdio.h>
#include "images.h"
#include "screen.h"
#include "game.h"
#include "hitbox.h"
#include "physics.h"
#include "st7789.h"
#define MAXSPEED 40

static int MapSlideIdx=0;
static int PipesSlideIdx=-180;
static int slideSpeed=5;
const int acceleration=2;
static int game_over = 0;  // 게임 오버 상태

// 비행기 위치
static int airplane_x = 50;
static int airplane_y = 50;

// 비행기 위치 범위 체크 함수
static int airplaneInRange(int x, int y) {
    // 화면 범위 내에 있는지 확인
    if (screen_bitmap.bitmap == NULL || airplane_bitmap.bitmap == NULL) {
        return 0;
    }
    if (screen_bitmap.width == 0 || screen_bitmap.height == 0) {
        return 0;
    }
    if (airplane_bitmap.width == 0 || airplane_bitmap.height == 0) {
        return 0;
    }
    return (y >= 0 && y + airplane_bitmap.height < screen_bitmap.height &&
            x >= 0 && x + airplane_bitmap.width < screen_bitmap.width);
}

void reset_game() {
    MapSlideIdx=0;
    PipesSlideIdx=-180;

    slideSpeed=5;
    game_over = 0;

    airplane_x = 50;
    airplane_y = 50;
}

void map_slide() {
    if (bgMap_bitmap.bitmap == NULL || bgMap_bitmap.width == 0) {
        return;
    }
    MapSlideIdx += slideSpeed;

    const int w = bgMap_bitmap.width;
    if (w > 0) {
        MapSlideIdx %= w;
    }

    set_map(&bgMap_bitmap, MapSlideIdx);
}

void pipes_slide() {
    if (pipes_bitmap.bitmap == NULL || pipes_bitmap.width == 0) {
        return;
    }
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


// 게임 오버 화면 표시
void show_game_over() {
    reset_screen();
    set_obj(0, 0, &gameOver_bitmap);
    draw_screen();
}

// 게임 오버 상태 확인
int is_game_over() {
    return game_over;
}

// 비행기 업데이트 (조이스틱 입력 처리)
void update_airplane() {
    moveObj(&airplane_x, &airplane_y, 4, airplaneInRange);
}

// 비행기 그리기
void draw_airplane() {
    set_obj(airplane_x, airplane_y, &airplane_bitmap);
}

// 게임 플레이 (모든 게임 로직 포함)
void play_game() {
    // 게임 오버 상태 처리
    if (game_over) {
        // 게임 오버 화면 표시 (매 프레임 갱신)
        show_game_over();
        
        // A와 B 버튼 동시 입력 체크
        ButtonState buttons = st7789_readButtons();
        if (buttons.a && buttons.b) {
            // 게임 재시작
            reset_game();
        }
        return;
    }
    
    // 화면 초기화
    reset_screen();
    
    // 배경 스크롤
    map_slide();
    
    // 파이프 스크롤
    pipes_slide();
    
    // 조이스틱으로 비행기 이동
    update_airplane();
    
    if (update_hitbox_and_check_collision(PipesSlideIdx, airplane_x, airplane_y)) {
        // 충돌 발생 - 게임 오버 처리
        game_over = 1;
        show_game_over();
        return;
    }
    
    // 비행기 그리기
    draw_airplane();
    
    // 화면에 출력
    draw_screen();
    
    // 가속 업데이트
    update_acceleration();
}