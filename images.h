#pragma once

#include <stdint.h>

typedef struct bitmap {
    uint16_t width;
    uint16_t height;
    uint16_t* bitmap;
} bitmap;

extern const bitmap pipes_bitmap;
extern const bitmap airplane_bitmap;
extern const bitmap gameOver_bitmap;
extern const bitmap bgMap_bitmap;
extern const bitmap screen_bitmap;
extern const bitmap hitbox_bitmap;