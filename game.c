#include<stdio.h>
#include "images.h"
#include "screen.h"
#include "game.h"
#include "hitbox.h"
#include "physics.h"
#include "st7789.h"
#define MAXSPEED 50
#define FIXRATE 60


#define DASH_FRAMES 3
#define DASH_SPEED  20

static int MapSlideIdx=0;
static int PipesSlideIdx=-180;
static int slideSpeed=5;
static int game_over = 0;  // 게임 오버 상태
const int airplaneSpeed=5;

// 비행기 위치
static int airplane_x = 50;
static int airplane_y = 50;

// 대쉬 상태
static int is_dashing = 0;
static int dash_frames_left = 0;
static ButtonState prev_buttons = {0, 0};

// 비행기 위치 범위 체크 함수
static int airplaneInRange(int x, int y) {
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

    // 대쉬 상태 초기화
    is_dashing = 0;
    dash_frames_left = 0;
    prev_buttons.a = 0;
    prev_buttons.b = 0;
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
    if (!(cnt%=FIXRATE)&&slideSpeed<=MAXSPEED) {
        slideSpeed++;
    }
}


// 게임 오버 화면 표시
void show_game_over() {
    set_obj(0, 0, &gameOver_bitmap);
    draw_screen();
}



// 비행기 업데이트 (조이스틱 입력 처리)
static void update_airplane(int speed) {
    moveObj(&airplane_x, &airplane_y, speed, airplaneInRange);
}

// 비행기 그리기
void draw_airplane() {
    set_obj(airplane_x, airplane_y, &airplane_bitmap);
}

// 게임 플레이 (모든 게임 로직 포함)
void play_game() {
    // 버튼 상태 공통 획득
    ButtonState buttons = st7789_readButtons();

    // 게임 오버 상태 처리
    if (game_over) {
        // 게임 오버 화면 표시 (매 프레임 갱신)
        show_game_over();
        

        if (buttons.a && buttons.b) {
            reset_game();
        }
        prev_buttons = buttons;
        return;
    }

    if (!is_dashing && buttons.a && !prev_buttons.a) {
        is_dashing = 1;
        dash_frames_left = DASH_FRAMES;
    }
    
    // 화면 초기화
    reset_screen();
    
    // 배경 스크롤
    map_slide();
    
    // 파이프 스크롤
    pipes_slide();
    
    // 조이스틱으로 비행기 이동 (대쉬 중이면 빠르게)
    if (is_dashing) {
        update_airplane(DASH_SPEED);
        dash_frames_left--;
        if (dash_frames_left <= 0) {
            is_dashing = 0;
        }
    } else {
        update_airplane(airplaneSpeed);
    }
    
    // 대쉬 중에는 충돌 판정 건너뛰기 (무적 대쉬)
    if (!is_dashing) {
        if (update_hitbox_and_check_collision(PipesSlideIdx, airplane_x, airplane_y)) {
            game_over = 1;
            show_game_over();
            prev_buttons = buttons;
            return;
        }
    }
    
    // 비행기 그리기
    draw_airplane();
    
    // 화면에 출력
    draw_screen();
    
    // 가속 업데이트
    update_acceleration();

    // 버튼 상태 저장 (다음 프레임에서 에지 검출용)
    prev_buttons = buttons;
}