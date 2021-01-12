#pragma once

#include <cstring>
#include "../World/State.h"
#include "../World/Chunk.h"
#include "Physics.h"

class Model;
extern Model *g;

void record_block(int x, int y, int z, int w);

int get_block(int x, int y, int z);

void toggle_light(int x, int y, int z);

void set_light(int p, int q, int x, int y, int z, int w);

void _set_block(int p, int q, int x, int y, int z, int w, int dirty);

void set_block(int x, int y, int z, int w);

void on_left_click();

void on_right_click();

void on_middle_click();

void on_key(GLFWwindow* window, int key, int scancode, int action, int mods);

void on_char(GLFWwindow* window, unsigned int u);

void on_scroll(GLFWwindow* window, double xdelta, double ydelta);

void on_mouse_button(GLFWwindow* window, int button, int action, int mods);

void handle_mouse_input();

void handle_movement(double dt);
