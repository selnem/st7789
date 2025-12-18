#pragma once

#include "st7789.h"

// 대쉬 / Invincibility 관련 상수
#define DASH_FRAMES 3
#define DASH_SPEED  20

#define INVINCIBILITY_FRAMES 60       
#define MAX_INVINCIBILITY_COUNT 3        

// 맵 슬라이딩 / 가속 관련 상수
#define MAP_MAXSPEED 50
#define MAP_FIXRATE  60

// 기본 이동
int moveObj(int *x, int *y, int delta);

// 초기화
void resetStatus();
//비행기 관련 함수
void updateInv(ButtonState now, ButtonState prev);
void airplaneMove(int *x, int *y, int baseSpeed, ButtonState now, ButtonState prev);

int blinkOn();

// 맵 슬라이딩 / 가속
void worldReset();
void slideMap();
void slidePipes();
void updateAccel();

// 충돌 체크 (히트박스 기반, 게임 래퍼)
int checkGameCollision(int airplaneX, int airplaneY);

// Invincibility UI 그리기 (우상단 아이콘)
void drawInvIcons();
