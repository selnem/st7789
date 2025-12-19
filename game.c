#include "images.h"
#include "screen.h"
#include "game.h"
#include "physics.h"
#include "st7789.h"
static int gameOver = 0;

// 비행기 위치
static int airplane_x = 50;
static int airplane_y = 50;

// 버튼 이전 상태
static ButtonState prevButtons = {0, 0};

void resetGame() {
    gameOver = 0;
    airplane_x = 50;
    airplane_y = 50;

    prevButtons.a = 0;
    prevButtons.b = 0;

    resetStatus();
    worldReset();
}


// 게임 오버 화면 표시
void showGameOver() {
    setObj(0, 0, &gameOver_bitmap);
    drawScreen();
}


// 비행기 그리기
void drawAirplane() {
    if (!blinkOn()) {
        return;
    }
    setObj(airplane_x, airplane_y, &airplane_bitmap);
}


static int handleGameOver(ButtonState buttons) {
    if (!gameOver) {
        return 0;
    }

    showGameOver();

    if (buttons.a && buttons.b) {
        resetGame();
    }

    // 버튼 상태 저장
    prevButtons = buttons;
    return 1;
}


static void updateMap() {
    resetScreen();

    slideMap();
    slidePipes();
    updateAccel();
}

// 충돌 처리
static int handleCollision(ButtonState buttons) {
    if (checkGameCollision(airplane_x, airplane_y)) {
        gameOver = 1;
        showGameOver();
        prevButtons = buttons;
        return 1;
    }
    return 0;
}
void frameUpdate(){
    drawAirplane();
    drawInvIcons();
    drawScreen();
}

// 게임 플레이
void playGame() {
    ButtonState buttons = st7789_readButtons();

    if (handleGameOver(buttons)) {
        return;
    }

    // 화면/배경 갱신
    updateMap();

    // 무적 갱신
    updateInv(buttons, prevButtons);

    // 비행기 이동
    airplaneMove(&airplane_x, &airplane_y, AIRPLANE_SPEED,
                 buttons, prevButtons);

    // 충돌 처리
    if (handleCollision(buttons)) {
        return;
    }

    frameUpdate();

    prevButtons = buttons;
}
