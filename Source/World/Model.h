#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../Worker/Worker.h"
#include "Chunk.h"
#include "State.h"

constexpr int MAX_CHUNKS = 8192;
constexpr int MAX_PLAYERS = 128;
constexpr int WORKERS = 4;
constexpr int MAX_TEXT_LENGTH = 256;
constexpr int MAX_PATH_LENGTH = 256;

struct Block {
	int x;
	int y;
	int z;
	int w;
};

class Model {
public:
	GLFWwindow* window;
	Worker workers[WORKERS];
	Chunk chunks[MAX_CHUNKS];
	int chunk_count;
	int create_radius;
	int render_radius;
	int delete_radius;
	State state;
	int typing;
	char typing_buffer[MAX_TEXT_LENGTH];
	int width;
	int height;
	int flying;     // 是否在飞
	int item_index;
	int scale;
	int ortho;
	float fov;
	int suppress_char;
	int day_length;
	int time_changed;
	Block block0;
	Block block1;
	static Model m_Instance;
	static Model& getInstance()
	{
		return m_Instance;
	}
};

extern Model* g;

inline void reset_model() {
    memset(g->chunks, 0, sizeof(Chunk) * MAX_CHUNKS);
    g->chunk_count = 0;
    
    g->flying = 0;
    g->item_index = 0;
    memset(g->typing_buffer, 0, sizeof(char) * MAX_TEXT_LENGTH);
    g->typing = 0;
    g->day_length = DAY_LENGTH;
    glfwSetTime(g->day_length / 3.0);
    g->time_changed = 1;
}