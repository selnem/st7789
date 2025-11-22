#include"st7789.h"

int moveObj(int *x, int *y,int delta,int(*inRange)(int,int)) {
  int isMove=0;
    JoystickState s = st7789_readJoystick();
    if (s.up) {
        if (inRange(*x, *y - delta)) {
            *y -= delta;
            isMove = 1;
        }
    }
    if (s.down) {
        if (inRange(*x, *y + delta)) {
            *y += delta;
            isMove = 1;
        }
    }
    if (s.left) {
        if (inRange(*x - delta, *y)) {
            *x -= delta;
            isMove = 1;
        }
    }
    if (s.right) {
        if (inRange(*x + delta, *y)) {
            *x += delta;
            isMove = 1;
        }
    }
    return isMove;
}