#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>

#include "noise.h"

#include "../Utils/Setting.h"
#include "../Utils/Inventory.h"
#include "../Utils/Tools.h"
#include "../Game/Physics.h"
#include "../Cube/MakeCube.h"
#include "../Maths/MatrixMath.h"
#include "../DataBase/Database.h"
#include "Map.h"

#ifdef _MSC_VER
#undef max(a,b)
#undef min(a,b)
#endif // _MSC_VER

class Model;
extern Model* g;

class Worker;
class WorkerItem;


struct Chunk {
	Map map;
	Map lights;
	int p;
	int q;
	int faces;
	int sign_faces;
	int dirty;
	int miny;       // 最低高度
	int maxy;       // 最高高度
	GLuint buffer;      // 物体的VBO
	GLuint sign_buffer;
	GLuint VAO;
};


Chunk* find_chunk(int p, int q);

// chunk和某个坐标在x轴和z轴上的最大距离
int chunk_distance(Chunk *chunk, int p, int q);

int chunk_visible(float planes[6][4], int p, int q, int miny, int maxy);

int highest_block(float x, float z);

int chunked(float x);

int has_lights(Chunk *chunk);

void dirty_chunk(Chunk *chunk);

void compute_chunk(WorkerItem *item);

void light_fill(
    bool *opaque, int *light,
    int x, int y, int z, int w, int force);

void generate_chunk(Chunk *chunk, WorkerItem *item);

void gen_chunk_buffer(Chunk *chunk);

void load_chunk(WorkerItem *item);

void init_chunk(Chunk* chunk, int p, int q);

void create_chunk(Chunk* chunk, int p, int q);

void delete_chunks();

void delete_all_chunks();

void ensure_chunks();
