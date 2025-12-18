#pragma once

// 게임 관련 상수 정의
#define MAXSPEED 50
#define FIXRATE 60

#define DASH_FRAMES 3
#define DASH_SPEED  20

// Invincibility 관련 설정
#define INVINCIBILITY_FRAMES 60          // 한 번 발동 시 지속 프레임 수
#define MAX_INVINCIBILITY_COUNT 3        // Invincibility 사용 가능 횟수

// 게임 인터페이스
void reset_game();  // 게임 초기화
void play_game();   // 게임 플레이 (모든 게임 로직 포함, 게임 오버 처리 포함)

