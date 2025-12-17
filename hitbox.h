#pragma once

void reset_hitbox_screen();
void set_hitbox_pipes(const bitmap* obj, int idx);
int set_hitbox_airplane(int x, int y, const bitmap* obj);  // 반환값: 충돌 여부 (1=충돌, 0=없음)
int check_collision();
int check_collision_at_position(int x, int y, const bitmap* obj);

