#include <stdint.h>
#include "images.h"
#include "st7789.h"

static uint16_t screen_pixels[ST7789_TFTWIDTH*ST7789_TFTWIDTH]={0};

static const uint16_t clr_circle_pixels[16] = {
  0x0000, 0xf800, 0xf800, 0x0000,
  0x07e0, 0x0000, 0x0000, 0xffe0,
  0x07e0, 0x0000, 0x0000, 0xffe0, 
  0x0000, 0x001f, 0x001f, 0x0000
};

const bitmap clr_circle_bitmap = { 4, 4, (uint16_t*)clr_circle_pixels };
const bitmap screen_bitmap={ST7789_TFTHEIGHT,ST7789_TFTWIDTH, (uint16_t*)screen_pixels};