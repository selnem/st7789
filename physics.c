#include "physics.h"
#include "images.h"
#include "screen.h"
#include "hitbox.h"
//대쉬
static int dashing = 0;
static int dash_left = 0;
//무적
static int invincible = 0;
static int inv_left = 0;
static int inv_cnt = MAX_INVINCIBILITY_COUNT;


// 내부 맵 슬라이딩 상태
static int map_idx = 0;
static int pipes_idx = -180;
static int slideSpeed = 5;

// 비행기 위치가 화면 안에 있는지 검사
static inline int inRange(int x, int y) {
    return (y >= 0 && y + airplane_bitmap.height < screen_bitmap.height &&
            x >= 0 && x + airplane_bitmap.width  < screen_bitmap.width);
}

int moveObj(int *x, int *y, int delta) {
    if (x == NULL || y == NULL) return 0;

    int moved = 0;
    JoystickState s = st7789_readJoystick();

    if (s.up) {
        if (inRange(*x, *y - delta)) {
            *y -= delta;
            moved = 1;
        }
    }
    if (s.down) {
        if (inRange(*x, *y + delta)) {
            *y += delta;
            moved = 1;
        }
    }
    if (s.left) {
        if (inRange(*x - delta, *y)) {
            *x -= delta;
            moved = 1;
        }
    }
    if (s.right) {
        if (inRange(*x + delta, *y)) {
            *x += delta;
            moved = 1;
        }
    }
    return moved;
}


void resetStatus() {
    dashing = 0;
    dash_left = 0;
    invincible = 0;
    inv_left = 0;
    inv_cnt = MAX_INVINCIBILITY_COUNT;
}

void updateInv(ButtonState now, ButtonState prev) {
    if (!invincible && inv_cnt > 0 && now.b && !prev.b) {
        invincible = 1;
        inv_left = INVINCIBILITY_FRAMES;
        inv_cnt--;
    }

    // Invincibility 타이머 감소
    if (invincible) {
        if (--inv_left <= 0) {
            invincible = 0;
        }
    }
}

void airplaneMove(int *x, int *y, int baseSpeed,
                  ButtonState now, ButtonState prev) {
    if (x == NULL || y == NULL) return;

    // A 키 에지 입력으로 대쉬 시작
    if (!dashing && now.a && !prev.a) {
        dashing = 1;
        dash_left = DASH_FRAMES;
    }

    // 이동 처리 (대쉬 중이면 속도 증가)
    if (dashing) {
        moveObj(x, y, DASH_SPEED);
        if (--dash_left <= 0) {
            dashing = 0;
        }
    } else {
        moveObj(x, y, baseSpeed);
    }
}

int blinkOn() {
    if (!invincible) return 1;
    return (inv_left % 6) >= 3; 
}


void worldReset() {
    map_idx = 0;
    pipes_idx = -180;
    slideSpeed = 5;
}

void slideMap() {
    if (bgMap_bitmap.bitmap == NULL || bgMap_bitmap.width == 0) {
        return;
    }
    map_idx += slideSpeed;

    const int w = bgMap_bitmap.width;
    if (w > 0) {
        map_idx %= w;
    }

    setMap(&bgMap_bitmap, map_idx);
}

void slidePipes() {
    if (pipes_bitmap.bitmap == NULL || pipes_bitmap.width == 0) {
        return;
    }
    pipes_idx += slideSpeed;

    const int w = pipes_bitmap.width;
    if (w > 0) {
        pipes_idx %= w;
    }

    setMapPipes(&pipes_bitmap, pipes_idx);
}

void updateAccel() {
    static int cnt = 0;
    cnt++;
    
    if (!(cnt %= MAP_FIXRATE) && slideSpeed <= MAP_MAXSPEED) {
        slideSpeed++;
    }
}


int checkGameCollision(int airplaneX, int airplaneY) {
    // 대쉬 중이거나 무적 상태일 때는 충돌 판정 건너뜀
    if (dashing || invincible) {
        return 0;
    }
    // 실제 히트박스 충돌은 hitbox.c 의 checkCollision 사용
    return checkCollision(pipes_idx, airplaneX, airplaneY);
}



void drawInvIcons() {
    if (inv_cnt <= 0) {
        return;
    }

    const int margin = 2;
    int icon_w = airplane_bitmap.width;
    int x = screen_bitmap.width - margin;
    int y = margin;
    int max = MAX_INVINCIBILITY_COUNT;

    for (int i = 0; i < max; i++) {
        x -= icon_w;
        if (i >= max - inv_cnt) {
            setObj(x, y, &airplane_bitmap);
        }
        x -= margin;
    }
}
