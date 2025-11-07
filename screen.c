#include <stdint.h>
#include <bcm2835.h>
#include "images.h"
#include "st7789.h"
#define reset_color 0x0000;
static uint16_t screen_pixels[ST7789_TFTWIDTH*ST7789_TFTWIDTH]={0};
const bitmap screen_bitmap={ST7789_TFTHEIGHT,ST7789_TFTWIDTH, (uint16_t*)screen_pixels};



static int inRange(int x,int y,const bitmap* obj);
void reset_screen(){
    for(int i=0;i<screen_bitmap.height;i++){
        for(int j=0;j<screen_bitmap.width;j++){
            screen_bitmap.bitmap[i*screen_bitmap.width+j]=reset_color;
        }
    }
}
void set_obj(uint16_t x, uint16_t y,const bitmap* obj){
  if(obj==NULL){
   return;
  }
  if(!inRange(x,y,obj)){
    return;
  }
  for(int i=0;i<obj->height;i++){
      for(int j=0;j<obj->width;j++){
        screen_bitmap.bitmap[(x+i)*screen_bitmap.width+(y+j)]=obj->bitmap[i*obj->width+j];
      }
  }
}

uint8_t pixelHi(uint16_t dot){
   return  dot>>8;
}
uint8_t pixelLo(uint16_t dot){
  return dot&0xFF;
}

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

  for(int i=0;i<ST7789_TFTWIDTH*ST7789_TFTWIDTH;i++){
    for(int j=0;j<ST7789_TFTHEIGHT;j++){
      bcm2835_spi_transfer(pixelHi(screen_pixels[i*ST7789_TFTWIDTH+j]));
      bcm2835_spi_transfer(pixelLo(screen_pixels[i*ST7789_TFTWIDTH+j]));
    }
  }

}

static int inRange(int x,int y,const bitmap* obj){
  return 0<=x && x+obj->width<screen_bitmap.width &&0<=y && y+obj->height<screen_bitmap.height;
}