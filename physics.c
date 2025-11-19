#include"st7789.h"

int moveDot(int *x, int *y,int xDelta,int yDelta,int(*inRange)(int,int)) {
  int isMove=0;
    JoystickState s = st7789_readJoystick();
    if (s.up) {
        if (inRange(*x, *y - yDelta)) {
            *y -= yDelta;
            isMove = 1;
        }
    }
    if (s.down) {
        if (inRange(*x, *y + yDelta)) {
            *y += yDelta;
            isMove = 1;
        }
    }
    if (s.left) {
        if (inRange(*x - xDelta, *y)) {
            *x -= xDelta;
            isMove = 1;
        }
    }
    if (s.right) {
        if (inRange(*x + xDelta, *y)) {
            *x += xDelta;
            isMove = 1;
        }
    }
    return isMove;
}