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

void reset_hitbox_screen() {
    for(int i = 0; i < hitbox_bitmap.height; i++) {
        for(int j = 0; j < hitbox_bitmap.width; j++) {
            hitbox_bitmap.bitmap[i * hitbox_bitmap.width + j] = 0x0000;
        }
    }
}

void set_hitbox_pipes(const bitmap* obj, int idx) {
    if (obj == NULL) {
        return;
    }
    // 파이프를 빨강으로 히트박스에 그리기
    for (int i = 0; i < obj->height && i < hitbox_bitmap.height; i++) {
        for (int j = 0; j < hitbox_bitmap.width; j++) {
            int source_idx = j + idx;
            if (source_idx >= 0) {
                source_idx %= obj->width;  // screen의 set_map_pipes와 동일한 원형 처리
                uint16_t pixel = obj->bitmap[oneDToTwoD_hitbox(i, source_idx, obj)];
                // 0x0000 픽셀은 투명이므로 무시, 그 외는 빨강으로 표시
                if (pixel != 0x0000) {
                    hitbox_bitmap.bitmap[oneDToTwoD_hitbox(i, j, &hitbox_bitmap)] = HITBOX_COLOR_PIPE;
                }
            }
        }
    }
}

int set_hitbox_airplane(int x, int y, const bitmap* obj) {
    if (obj == NULL) {
        return 0;
    }
    int collision = 0;
    // 비행기를 녹색으로 히트박스에 그리기 (그리면서 충돌 확인)
    for (int i = 0; i < obj->height && (x + i) < hitbox_bitmap.height; i++) {
        if (x + i >= 0) {
            for (int j = 0; j < obj->width && (y + j) < hitbox_bitmap.width; j++) {
                if (y + j >= 0) {
                    uint16_t pixel = obj->bitmap[oneDToTwoD_hitbox(i, j, obj)];
                    if (pixel != 0x0000) {
                        int pos = oneDToTwoD_hitbox(x + i, y + j, &hitbox_bitmap);
                        // 비행기를 그리기 전에 파이프(빨강)가 있는지 확인
                        if (hitbox_bitmap.bitmap[pos] == HITBOX_COLOR_PIPE) {
                            collision = 1;  // 충돌 발생
                        }
                        // 비행기를 녹색으로 표시
                        hitbox_bitmap.bitmap[pos] = HITBOX_COLOR_AIRPLANE;
                    }
                }
            }
        }
    }
    return collision;
}

int check_collision() {

    (void)hitbox_bitmap;
    return 0;
}

int check_collision_at_position(int x, int y, const bitmap* obj) {
    if (obj == NULL) {
        return 0;
    }
    // 비행기 위치에서 파이프(빨강)와 겹치는지 확인
    for (int i = 0; i < obj->height && (x + i) < hitbox_bitmap.height; i++) {
        if (x + i >= 0) {
            for (int j = 0; j < obj->width && (y + j) < hitbox_bitmap.width; j++) {
                if (y + j >= 0) {
                    uint16_t pixel = obj->bitmap[oneDToTwoD_hitbox(i, j, obj)];
                    // 비행기의 실제 픽셀(0x0000이 아닌) 위치에서
                    if (pixel != 0x0000) {
                        // 히트박스 버퍼에 파이프(빨강)가 있는지 확인
                        uint16_t hitbox_pixel = hitbox_bitmap.bitmap[oneDToTwoD_hitbox(x + i, y + j, &hitbox_bitmap)];
                        if (hitbox_pixel == HITBOX_COLOR_PIPE) {
                            return 1;  // 충돌 발생
                        }
                    }
                }
            }
        }
    }
    return 0;  // 충돌 없음
}
