#pragma once

#include "../World/Map.h"

void get_sight_vector(float rx, float ry, float *vx, float *vy, float *vz);
void get_motion_vector(int flying, int sz, int sx, float rx, float ry,
                       float *vx, float *vy, float *vz);

int _hit_test(Map* map, float max_distance, int previous, float x, float y, float z, float vx, float vy, float vz, int* hx, int* hy, int* hz);

int hit_test(int previous, float x, float y, float z, float rx, float ry, int* bx, int* by, int* bz);

int collide(int height, float* x, float* y, float* z);

int player_intersects_block(int height, float x, float y, float z, int hx, int hy, int hz);

void occlusion(char neighbors[27], int lights[27], float shades[27], float ao[6][4], float light[6][4]);

class Model;
extern Model* g;