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
inline int oneDToTwoD(int i,int j, const bitmap *obj) {
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
        screen_bitmap.bitmap[oneDToTwoD(x+i,y+j,&screen_bitmap)]=obj->bitmap[oneDToTwoD(i,j,obj)];
      }
  }
}


void set_map(const bitmap* obj, int idx) {
  if (obj==NULL) {
    return;
  }
  for (int i=0;i<obj->height;i++) {
    for (int j=0;j<screen_bitmap.width;j++) {
      screen_bitmap.bitmap[oneDToTwoD(i,j,&screen_bitmap)]=obj->bitmap[oneDToTwoD(i,j+idx,obj)];
    }
  }
}

static inline int inRange(int x,int y,const bitmap* obj){
  return 0<=x && x+obj->width<screen_bitmap.width &&0<=y && y+obj->height<screen_bitmap.height;
}






/*
void test_screen(){
  uint16_t bg_color=0x0000;
  reset_screen();
  set_obj(30,30,&clr_circle_bitmap);
  // Set full-screen address window
  writeCommand(ST7789_RASET);
  writeData(0); writeData(0);
  writeData((ST7789_TFTHEIGHT >> 8) & 0xFF); writeData(ST7789_TFTHEIGHT & 0xFF);

  writeCommand(ST7789_CASET);
  writeData(0); writeData(0);
  writeData((ST7789_TFTWIDTH >> 8) & 0xFF); writeData(ST7789_TFTWIDTH & 0xFF);

  writeCommand(ST7789_RAMWR);
  bcm2835_gpio_set(TFT_DC);

  for(int i=0;i<ST7789_TFTHEIGHT;i++){
    for(int j=0;j<ST7789_TFTWIDTH;j++){
      bcm2835_spi_transfer(pixelHi(screen_bitmap.bitmap[i*ST7789_TFTWIDTH+j]));
      bcm2835_spi_transfer(pixelLo(screen_bitmap.bitmap[i*ST7789_TFTWIDTH+j]));
    }
  }
}
*/