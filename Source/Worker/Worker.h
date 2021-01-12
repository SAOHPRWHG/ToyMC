#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../World/Map.h"
#include "../World/Chunk.h"
#include "tinycthread.h"

#define WORKER_IDLE 0
#define WORKER_BUSY 1
#define WORKER_DONE 2

#ifdef _MSC_VER
#undef max(a,b)
#undef min(a,b)
#endif

class Model;
extern Model* g;

struct WorkerItem {
	int p;
	int q;
	int load;
	Map* block_maps[3][3];
	Map* light_maps[3][3];
	int miny;
	int maxy;
	int faces;
	GLfloat* data;
};

struct Worker {
	int index;
	int state;
	thrd_t thrd;
	mtx_t mtx;
	cnd_t cnd;
	WorkerItem item;
};

void check_workers();
void ensure_chunks_worker(Worker* worker);
int worker_run(void* arg);
