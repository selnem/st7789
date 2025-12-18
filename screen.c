#include <stdint.h>
#include <bcm2835.h>
#include <unistd.h>
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


void resetScreen(){
    for(int i=0;i<screen_bitmap.height;i++){
        for(int j=0;j<screen_bitmap.width;j++){
            int idx = i*screen_bitmap.width+j;
            screen_bitmap.bitmap[idx]=reset_color;
        }
    }
}

void drawScreen() {
  st7789_writeCmds();
  for(int i=0;i<screen_bitmap.height && i<ST7789_TFTHEIGHT;i++){
    for(int j=0;j<screen_bitmap.width && j<ST7789_TFTWIDTH;j++){
      int idx = i*screen_bitmap.width+j;
      bcm2835_spi_transfer(pixelHi(screen_bitmap.bitmap[idx]));
      bcm2835_spi_transfer(pixelLo(screen_bitmap.bitmap[idx]));
    }
 
  }
}
static inline int oneDToTwoD(int i,int j, const bitmap *obj) {
  return obj->width*i+j;
}

void setObj(int x, int y,const bitmap* obj){

  if(!inRange(x,y,obj)){
    return;
  }
  for(int i=0;i<obj->height;i++){
      for(int j=0;j<obj->width;j++){
        int src_idx = oneDToTwoD(i,j,obj);
        uint16_t pixel = obj->bitmap[src_idx];
        // 0x0000 픽셀은 투명이므로 무시
        if(pixel != 0x0000) {
          int dst_idx = oneDToTwoD(y+i,x+j,&screen_bitmap);
          if (dst_idx >= 0 && dst_idx < screen_bitmap.width * screen_bitmap.height) {
            screen_bitmap.bitmap[dst_idx] = pixel;
          }
        }
      }
  }
}


void setMap(const bitmap* obj, int idx) {
  int width = obj->width;

  // idx를 0 ~ width-1 범위로 정규화해서 원형 스크롤
  int base_idx = idx % width;
  if (base_idx < 0) {
    base_idx += width;
  }
  for (int i=0;i<obj->height;i++) {
    for (int j=0;j<screen_bitmap.width;j++) {
      int col = (base_idx + j) % width;
      int src_idx = oneDToTwoD(i,col,obj);
      uint16_t pixel = obj->bitmap[src_idx];
      if(pixel != 0x0000) {
        int dst_idx = oneDToTwoD(i,j,&screen_bitmap);
        if (dst_idx >= 0 && dst_idx < screen_bitmap.width * screen_bitmap.height) {
          screen_bitmap.bitmap[dst_idx] = pixel;
        }
      }
    }
  }
}

void setMapPipes(const bitmap* obj, int idx) {

  for (int i=0;i<obj->height && i<screen_bitmap.height;i++) {
    for (int j=0;j<screen_bitmap.width;j++) {
      // idx가 음수인 경우 (파이프가 화면 오른쪽에서 시작하는 경우) 처리
      int source_idx = j + idx;
      if (source_idx >= 0 ) {
        source_idx %= obj->width;
        int src_idx = oneDToTwoD(i,source_idx,obj);
        uint16_t pixel = obj->bitmap[src_idx];
        if(pixel != 0x0000) {
          int dst_idx = oneDToTwoD(i,j,&screen_bitmap);
          if (dst_idx >= 0 && dst_idx < screen_bitmap.width * screen_bitmap.height) {
            screen_bitmap.bitmap[dst_idx] = pixel;
          }
        }
      }
    }
  }
}

static inline int inRange(int x,int y,const bitmap* obj){
  return 0<=x && x+obj->width<=screen_bitmap.width &&0<=y && y+obj->height<=screen_bitmap.height;
}






