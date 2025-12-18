//
// Created by 전성환 on 25. 11. 28.
//

#include <stdint.h>
#include <stddef.h>
#include "images.h"
#include "screen.h"
#include "game.h"

#define HITBOX_COLOR_PIPE 0xF800  // 빨강
#define HITBOX_COLOR_AIRPLANE 0x07E0  // 녹색

static inline int oneDToTwoD_hitbox(int i, int j, const bitmap *obj) {
    return obj->width * i + j;
}

static inline int inHitboxRange(int x, int y) {
    return (y >= 0 && y < hitbox_bitmap.height &&
            x >= 0 && x < hitbox_bitmap.width);
}

void resetHitboxScreen() {
    for(int i = 0; i < hitbox_bitmap.height; i++) {
        for(int j = 0; j < hitbox_bitmap.width; j++) {
            hitbox_bitmap.bitmap[i * hitbox_bitmap.width + j] = 0x0000;
        }
    }
}

void setHitboxPipes(const bitmap* obj, int idx) {
    if (obj == NULL) {
        return;
    }
    // 파이프를 빨강으로 히트박스에 그리기
    for (int i = 0; i < obj->height && i < hitbox_bitmap.height; i++) {
        for (int j = 0; j < hitbox_bitmap.width; j++) {
            int source_idx = j + idx;
            if (source_idx >= 0) {
                source_idx %= obj->width;
                uint16_t pixel = obj->bitmap[oneDToTwoD_hitbox(i, source_idx, obj)];
                // 0x0000 픽셀은 투명이므로 무시, 그 외는 빨강으로 표시
                if (pixel != 0x0000) {
                    hitbox_bitmap.bitmap[oneDToTwoD_hitbox(i, j, &hitbox_bitmap)] = HITBOX_COLOR_PIPE;
                }
            }
        }
    }
}

int setHitboxAirplane(int x, int y, const bitmap* obj) {
    for (int i = 0; i < obj->height; i++) {
        for (int j = 0; j < obj->width; j++) {
            uint16_t pixel = obj->bitmap[oneDToTwoD_hitbox(i, j, obj)];
            if (pixel == 0x0000) continue;

            int hx = x + j;
            int hy = y + i;

            if (!inHitboxRange(hx, hy)) continue;

            int pos = oneDToTwoD_hitbox(hy, hx, &hitbox_bitmap);
            // 그리기 전에 파이프가 있는지 확인
            if (hitbox_bitmap.bitmap[pos] == HITBOX_COLOR_PIPE) return 1;
            // 비행기를 녹색으로 표시
            hitbox_bitmap.bitmap[pos] = HITBOX_COLOR_AIRPLANE;
        }
    }
    return 0;
}



// 히트박스 업데이트 및 충돌 체크 (파이프와 비행기 히트박스 업데이트 및 충돌 체크)
int checkCollision(int pipesSlideIdx, int airplaneX, int airplaneY) {
    // 히트박스 화면 초기화
    resetHitboxScreen();
    
    // 파이프를 히트박스에 표시
    setHitboxPipes(&pipes_bitmap, pipesSlideIdx);
    
    // 비행기를 히트박스에 표시하고 충돌 체크 (반환값: 1=충돌, 0=없음)
    return setHitboxAirplane(airplaneX, airplaneY, &airplane_bitmap);
}
