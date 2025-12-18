#include<stdio.h>
#include "images.h"
#include "screen.h"
#include "game.h"
#include "hitbox.h"
#include "physics.h"
#include "st7789.h"

static int MapSlideIdx=0;
static int PipesSlideIdx=-180;
static int slideSpeed=5;
static int game_over = 0;  // 게임 오버 상태
const int airplaneSpeed=5;

// 비행기 위치
static int airplane_x = 50;
static int airplane_y = 50;

// 대쉬 상태
static int dashing = 0;
static int dash_left = 0;
static ButtonState prev_buttons = {0, 0};

// Invincibility 상태
static int invincible = 0;
static int inv_left = 0;
static int inv_cnt = MAX_INVINCIBILITY_COUNT;  // 남은 Invincibility 사용 가능 횟수

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
    dashing = 0;
    dash_left = 0;
    prev_buttons.a = 0;
    prev_buttons.b = 0;

    // Invincibility 상태 초기화
    invincible = 0;
    inv_left = 0;
    inv_cnt = MAX_INVINCIBILITY_COUNT;
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

// Invincibility 남은 카운트 우상단에 표시
static void draw_invincibility_count() {
    // 사용 가능 횟수가 0이면 아무것도 표시하지 않음
    if (inv_cnt <= 0) {
        return;
    }

    const int margin = 2;
    int icon_w = airplane_bitmap.width;
    int x = screen_bitmap.width - margin;
    int y = margin;

    // 남은 횟수만큼 비행기 아이콘을 우상단에 오른쪽 정렬로 그린다
    for (int i = 0; i < inv_cnt; i++) {
        x -= icon_w;
        set_obj(x, y, &airplane_bitmap);
        x -= margin;
    }
}

// 비행기 그리기
void draw_airplane() {
    if (invincible) {
        if ((inv_left % 6) < 3) {
            return;
        }
    }
    set_obj(airplane_x, airplane_y, &airplane_bitmap);
}

// -------- 내부 게임 로직 헬퍼 함수들 --------

// 게임 오버 상태 처리
static int handle_game_over(ButtonState buttons) {
    if (!game_over) {
        return 0;
    }

    // 게임 오버 화면 표시 (매 프레임 갱신)
    show_game_over();

    // A + B 동시 입력 시 게임 리셋
    if (buttons.a && buttons.b) {
        reset_game();
    }

    // 버튼 상태 저장
    prev_buttons = buttons;
    return 1;
}


static void update_invincibility(ButtonState buttons) {
    // B 키를 눌렀을 때 Invincibility 시작 (에지 감지, 남은 카운트가 있을 때만)
    if (!invincible && inv_cnt > 0 && buttons.b && !prev_buttons.b) {
        invincible = 1;
        inv_left = INVINCIBILITY_FRAMES;
        inv_cnt--;
    }

    // Invincibility 시간 감소
    if (invincible) {
        inv_left--;
        if (inv_left <= 0) {
            invincible = 0;
        }
    }
}

// 대쉬 상태 갱신
static void update_dash(ButtonState buttons) {

    if (!dashing && buttons.a && !prev_buttons.a) {
        dashing = 1;
        dash_left = DASH_FRAMES;
    }

    if (dashing) {
        update_airplane(DASH_SPEED);
        dash_left--;
        if (dash_left <= 0) {
            dashing = 0;
        }
    } else {
        update_airplane(airplaneSpeed);
    }
}

static void update_map() {
    // 화면 초기화
    reset_screen();

    // 배경 스크롤
    map_slide();

    // 파이프 스크롤
    pipes_slide();
}

// 충돌 처리
static int handle_collision(ButtonState buttons) {
    // 대쉬 중이거나 Invincibility 상태일 때는 충돌 판정 건너뛰기
    if (!dashing && !invincible) {
        if (update_hitbox_and_check_collision(PipesSlideIdx, airplane_x, airplane_y)) {
            game_over = 1;
            show_game_over();
            prev_buttons = buttons;
            return 1;
        }
    }
    return 0;
}




// 게임 플레이 
void play_game() {
  
    ButtonState buttons = st7789_readButtons();

    if (handle_game_over(buttons)) {
        return;
    }

    update_invincibility(buttons);

    update_map();

    // 이동/대쉬 처리
    update_dash(buttons);

    if (handle_collision(buttons)) {
        return;
    }



   draw_airplane();

   draw_invincibility_count();

   draw_screen();


   update_acceleration();

    prev_buttons = buttons;
}