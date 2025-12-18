#pragma once

void reset_hitbox_screen();
void set_hitbox_pipes(const bitmap* obj, int idx);
int set_hitbox_airplane(int x, int y, const bitmap* obj);  // 반환값: 충돌 여부 (1=충돌, 0=없음)
int check_collision();
int update_hitbox_and_check_collision(int pipes_slide_idx, int airplane_x, int airplane_y);  // 히트박스 업데이트 및 충돌 체크, 반환값: 충돌 여부 (1=충돌, 0=없음)

