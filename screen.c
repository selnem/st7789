#include <stdint.h>
#include <bcm2835.h>
#include "images.h"
#include "st7789.h"
#define reset_color 0x0000;




static inline int inRange(int x,int y,const bitmap* obj);

static inline uint8_t pixelHi(uint16_t dot){
  return  dot>>8;
}

static inline uint8_t pixelLo(uint16_t dot){
  return dot&0xFF;
}


void reset_screen(){
    for(int i=0;i<screen_bitmap.height;i++){
        for(int j=0;j<screen_bitmap.width;j++){
            screen_bitmap.bitmap[i*screen_bitmap.width+j]=reset_color;
        }
    }
}

void draw_screen() {
  st7789_writeCmds();
  for(int i=0;i<ST7789_TFTHEIGHT;i++){
    for(int j=0;j<ST7789_TFTWIDTH;j++){
      bcm2835_spi_transfer(pixelHi(screen_bitmap.bitmap[i*ST7789_TFTWIDTH+j]));
      bcm2835_spi_transfer(pixelLo(screen_bitmap.bitmap[i*ST7789_TFTWIDTH+j]));
    }
  }
}
static inline int oneDToTwoD(int i,int j, const bitmap *obj) {
  return obj->width*i+j;
}

void set_obj(int x, int y,const bitmap* obj){
  if(obj==NULL){
   return;
  }
  if(!inRange(x,y,obj)){
    return;
  }
  for(int i=0;i<obj->height;i++){
      for(int j=0;j<obj->width;j++){
        uint16_t pixel = obj->bitmap[oneDToTwoD(i,j,obj)];
        // 0x0000 픽셀은 투명이므로 무시
        if(pixel != 0x0000) {
          screen_bitmap.bitmap[oneDToTwoD(y+i,x+j,&screen_bitmap)] = pixel;
        }
      }
  }
}


void set_map(const bitmap* obj, int idx) {
  if (obj==NULL) {
    return;
  }
  int width = obj->width;
  if (width <= 0) {
    return;
  }

  // idx를 0 ~ width-1 범위로 정규화해서 원형 스크롤
  int base_idx = idx % width;
  if (base_idx < 0) {
    base_idx += width;
  }

  for (int i=0;i<obj->height;i++) {
    for (int j=0;j<screen_bitmap.width;j++) {
      int col = (base_idx + j) % width;
      uint16_t pixel = obj->bitmap[oneDToTwoD(i,col,obj)];
      if(pixel != 0x0000) {
        screen_bitmap.bitmap[oneDToTwoD(i,j,&screen_bitmap)] = pixel;
      }
    }
  }
}

void set_map_pipes(const bitmap* obj, int idx) {
  if (obj==NULL) {
    return;
  }
  for (int i=0;i<obj->height && i<screen_bitmap.height;i++) {
    for (int j=0;j<screen_bitmap.width;j++) {
      // idx가 음수인 경우 (파이프가 화면 오른쪽에서 시작하는 경우) 처리
      int source_idx = j + idx;
      if (source_idx >= 0 ) {
        source_idx %=obj->width;
        uint16_t pixel = obj->bitmap[oneDToTwoD(i,source_idx,obj)];
        if(pixel != 0x0000) {
          screen_bitmap.bitmap[oneDToTwoD(i,j,&screen_bitmap)] = pixel;
        }
      }
    }
  }
}

static inline int inRange(int x,int y,const bitmap* obj){
  return 0<=x && x+obj->width<screen_bitmap.width &&0<=y && y+obj->height<screen_bitmap.height;
}






