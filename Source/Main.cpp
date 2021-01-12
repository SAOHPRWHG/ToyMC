#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <algorithm>
#include <memory>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include "./Utils/Setting.h"
#include "./Cube/MakeCube.h"
#include "./DataBase/Database.h"
#include "./Utils/Inventory.h"
#include "./World/Map.h"
#include "./Maths/MatrixMath.h"
#include "noise.h"
#include "./Utils/PromptSign.h"
#include "tinycthread.h"
#include "./Utils/Tools.h"
#include "./World/World.h"
#include "./World/Chunk.h"
#include "./World/Model.h"
#include "./Worker/Worker.h"
#include "./Game/Physics.h"
#include "./Game/KBMouse.h"

constexpr int SKY_BUFFER_LEN = 12300;
constexpr int CROSSHAIR_BUFFER_LEN = 8;
constexpr int BLOCK_BUFFER_LEN = 360;
constexpr int PLANT_SUBDATA_LEN = 240;


#define ALIGN_LEFT 0
#define ALIGN_CENTER 1
#define ALIGN_RIGHT 2

#ifdef _MSC_VER
#undef max(a,b)
#undef min(a,b)
#endif

struct Attrib {
	GLuint VAO;
	GLuint VBO;
	GLuint program;
	GLuint matrix;
	GLuint sampler;
	GLuint camera;
	GLuint timer;
	GLuint extra1;
	GLuint extra2;
	GLuint extra3;
	GLuint extra4;
	std::unique_ptr<GLfloat[]> buffer;
	~Attrib()
	{
		if (VAO)
		{
			glDeleteVertexArrays(1, &VAO);
		}
		if (VBO)
		{
			glDeleteBuffers(1, &VBO);
		}
	}
};

Model Model::m_Instance;
Model* g = &Model::getInstance();


float time_of_day() {
    if (g->day_length <= 0) {
        return 0.5;
    }
    float t;
    t = glfwGetTime();
    t = t / g->day_length;
    t = t - (int)t;
    return t;
}

float get_daylight() {
    float timer = time_of_day();
    if (timer < 0.5) {
        float t = (timer - 0.25) * 100;
        return 1 / (1 + pow(2, -t));
    }
    else {
        float t = (timer - 0.85) * 100;
        return 1 - 1 / (1 + pow(2, -t));
    }
}

int get_scale_factor() {
    int window_width, window_height;
    int buffer_width, buffer_height;
    glfwGetWindowSize(g->window, &window_width, &window_height);
    glfwGetFramebufferSize(g->window, &buffer_width, &buffer_height);
    int result = buffer_width / window_width;
    result = std::max(1, result);
    result = std::min(2, result);
    return result;
}


void gen_crosshair_bufferdata(GLfloat* data) {
    float x = g->width / 2;
    float y = g->height / 2;
    float p = 10 * g->scale;
    
	data[0] = x;
	data[1] = y - p;
	data[2] = x;
	data[3] = y + p;
	data[4] = x - p;
	data[5] = y;
	data[6] = x + p;
	data[7] = y;

}

// 生成普通方块并且送入缓存中
void gen_cube_bufferdata(GLfloat* data, int w) {
    float ao[6][4] = { {0} };
    float light[6][4] = {
        {0.5, 0.5, 0.5, 0.5},
        {0.5, 0.5, 0.5, 0.5},
        {0.5, 0.5, 0.5, 0.5},
        {0.5, 0.5, 0.5, 0.5},
        {0.5, 0.5, 0.5, 0.5},
        {0.5, 0.5, 0.5, 0.5}
    };
    make_cube(data, ao, light, 1, 1, 1, 1, 1, 1, 0.0f, 0.0f, 0.0f, 0.5f, w);
}

void draw_triangles_3d(Attrib *attrib, int count) {
    glBindVertexArray(attrib->VAO);
    glDrawArrays(GL_TRIANGLES, 0, count);
    glBindVertexArray(0);
}

void draw_lines(Attrib *attrib, int count) {
	glBindVertexArray(attrib->VAO);
	glDrawArrays(GL_LINES, 0, count);
	glBindVertexArray(0);
}

void draw_chunk(Attrib *attrib, Chunk *chunk) {
	glBindVertexArray(chunk->VAO);
	glDrawArrays(GL_TRIANGLES, 0, chunk->faces * 6);
	glBindVertexArray(0);
}

void draw_cube(Attrib *attrib) {
	draw_triangles_3d(attrib, 36);
}

void draw_plant(Attrib *attrib) {
	draw_triangles_3d(attrib, 24);
}

int render_chunks(Attrib *attrib) {
    int result = 0;
    State *s = &g->state;
    ensure_chunks();
    int p = chunked(s->x);
    int q = chunked(s->z);
    float light = get_daylight();
    float matrix[16];
    set_matrix_3d(
        matrix, g->width, g->height,
        s->x, s->y, s->z, s->rx, s->ry, g->fov, g->ortho, g->render_radius);
    float planes[6][4];
    frustum_planes(planes, g->render_radius, matrix);
    glUseProgram(attrib->program);
    glUniformMatrix4fv(attrib->matrix, 1, GL_FALSE, matrix);
    glUniform3f(attrib->camera, s->x, s->y, s->z);
    glUniform1i(attrib->sampler, 0);
    glUniform1i(attrib->extra1, 2);
    glUniform1f(attrib->extra2, light);
    glUniform1f(attrib->extra3, g->render_radius * CHUNK_SIZE);
    glUniform1i(attrib->extra4, g->ortho);
    glUniform1f(attrib->timer, time_of_day());
    for (int i = 0; i < g->chunk_count; i++) {
        Chunk *chunk = g->chunks + i;
        if (chunk_distance(chunk, p, q) > g->render_radius) {
            continue;
        }
        if (!chunk_visible(
            planes, chunk->p, chunk->q, chunk->miny, chunk->maxy))
        {
            continue;
        }
        draw_chunk(attrib, chunk);
        result += chunk->faces;
    }
    return result;
}

void render_sky(Attrib *attrib) {
	make_sphere(attrib->buffer.get(), 1, 3);

    State *s = &g->state;
    float matrix[16];
    set_matrix_3d(
        matrix, g->width, g->height,
        0, 0, 0, s->rx, s->ry, g->fov, 0, g->render_radius);
    glUseProgram(attrib->program);
    glUniformMatrix4fv(attrib->matrix, 1, GL_FALSE, matrix);
    glUniform1i(attrib->sampler, 2);
    glUniform1f(attrib->timer, time_of_day());

	glBindBuffer(GL_ARRAY_BUFFER, attrib->VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 12288 * sizeof(GLfloat), attrib->buffer.get());
    draw_triangles_3d(attrib, 512 * 3);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void render_crosshairs(Attrib *attrib) {
    float matrix[16];
    set_matrix_2d(matrix, g->width, g->height);
    glUseProgram(attrib->program);
    glLineWidth(2 * g->scale);
    glUniformMatrix4fv(attrib->matrix, 1, GL_FALSE, matrix);
	gen_crosshair_bufferdata(attrib->buffer.get());
	glBindBuffer(GL_ARRAY_BUFFER, attrib->VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, CROSSHAIR_BUFFER_LEN * sizeof(GLfloat), attrib->buffer.get());
    draw_lines(attrib, 10);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void render_item(Attrib *attrib) {
    float matrix[16];
    set_matrix_item(matrix, g->width, g->height, g->scale);
    glUseProgram(attrib->program);
    glUniformMatrix4fv(attrib->matrix, 1, GL_FALSE, matrix);
    glUniform3f(attrib->camera, 0, 0, 5);
    glUniform1i(attrib->sampler, 0);
    glUniform1f(attrib->timer, time_of_day());
    int w = Inventory[g->item_index];

	glBindBuffer(GL_ARRAY_BUFFER, attrib->VBO);

    if (is_plant(w)) {
		make_plant(attrib->buffer.get(), 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.5f, w, 45.0f);
		glBufferSubData(GL_ARRAY_BUFFER, 0, PLANT_SUBDATA_LEN * sizeof(GLfloat), attrib->buffer.get());
        draw_plant(attrib);
    }
    else {
		gen_cube_bufferdata(attrib->buffer.get(), w);
		glBufferSubData(GL_ARRAY_BUFFER, 0, BLOCK_BUFFER_LEN * sizeof(GLfloat), attrib->buffer.get());
        draw_cube(attrib);
    }
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int main(int argc, char **argv) {
    // INITIALIZATION //
    srand(time(NULL));
	rand();
	// glfw: initialize and configure
	// ------------------------------
    glfwInit(); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	g->window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "ToyMC", NULL, NULL);
	if (g->window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

    glfwMakeContextCurrent(g->window);
    glfwSwapInterval(VSYNC);
    glfwSetInputMode(g->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(g->window, on_key);
    glfwSetCharCallback(g->window, on_char);
    glfwSetMouseButtonCallback(g->window, on_mouse_button);
    glfwSetScrollCallback(g->window, on_scroll);

    // glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

    glEnable(GL_CULL_FACE); 
    glEnable(GL_DEPTH_TEST);
    glLogicOp(GL_INVERT);
    glClearColor(0, 0, 0, 1);

    // LOAD TEXTURES //
    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    load_png_texture("Textures/texture.png");

    GLuint sky;
    glGenTextures(1, &sky);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, sky);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    load_png_texture("Textures/sky.png");

    // LOAD SHADERS //
    Attrib block_attrib = {0};
    Attrib line_attrib = {0};
    Attrib text_attrib = {0};
    Attrib sky_attrib = {0};
    
    //SET VAO//
    //block_attrib//
    glGenVertexArrays(1, &block_attrib.VAO);
    glBindVertexArray(block_attrib.VAO);
	block_attrib.buffer = std::unique_ptr<GLfloat[]>(new GLfloat[BLOCK_BUFFER_LEN]);
	glGenBuffers(1, &block_attrib.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, block_attrib.VBO);
	glBufferData(GL_ARRAY_BUFFER, BLOCK_BUFFER_LEN * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        sizeof(GLfloat) * 10, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
        sizeof(GLfloat) * 10, (GLvoid *)(sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE,
        sizeof(GLfloat) * 10, (GLvoid *)(sizeof(GLfloat) * 6));
    glEnableVertexAttribArray(2);

    //sky_attrib//
    glGenVertexArrays(1, &sky_attrib.VAO);
    glBindVertexArray(sky_attrib.VAO);
	sky_attrib.buffer = std::unique_ptr<GLfloat[]>(new GLfloat[SKY_BUFFER_LEN]);
	glGenBuffers(1, &sky_attrib.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, sky_attrib.VBO);
	glBufferData(GL_ARRAY_BUFFER, SKY_BUFFER_LEN * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (GLvoid *)(sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (GLvoid *)(sizeof(GLfloat) * 6));
    glEnableVertexAttribArray(2);
	
	//line_attrib
	glGenVertexArrays(1, &line_attrib.VAO);
	glBindVertexArray(line_attrib.VAO);
	glGenBuffers(1, &line_attrib.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, line_attrib.VBO);
	line_attrib.buffer = std::unique_ptr<GLfloat[]>(new GLfloat[CROSSHAIR_BUFFER_LEN]);
	glBufferData(GL_ARRAY_BUFFER, CROSSHAIR_BUFFER_LEN * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
    GLuint program;
    program = load_program(
        "Shaders/block_render.vs", "Shaders/block_render.fs");
    block_attrib.program = program;
    block_attrib.matrix = glGetUniformLocation(program, "matrix");
    block_attrib.sampler = glGetUniformLocation(program, "sampler");
    block_attrib.extra1 = glGetUniformLocation(program, "sky_sampler");
    block_attrib.extra2 = glGetUniformLocation(program, "daylight");
    block_attrib.extra3 = glGetUniformLocation(program, "fog_distance");
    block_attrib.extra4 = glGetUniformLocation(program, "ortho");
    block_attrib.camera = glGetUniformLocation(program, "camera");
    block_attrib.timer = glGetUniformLocation(program, "timer");

    program = load_program(
        "Shaders/line.vs", "Shaders/line.fs");
    line_attrib.program = program;
    line_attrib.matrix = glGetUniformLocation(program, "matrix");

    program = load_program(
        "Shaders/text_render.vs", "Shaders/text_render.fs");
    text_attrib.program = program;
    text_attrib.matrix = glGetUniformLocation(program, "matrix");
    text_attrib.sampler = glGetUniformLocation(program, "sampler");
    text_attrib.extra1 = glGetUniformLocation(program, "is_sign");

    program = load_program(
        "Shaders/sky_lights.vs", "Shaders/sky_lights.fs");
    sky_attrib.program = program;
    sky_attrib.matrix = glGetUniformLocation(program, "matrix");
    sky_attrib.sampler = glGetUniformLocation(program, "sampler");
    sky_attrib.timer = glGetUniformLocation(program, "timer");


    g->create_radius = CREATE_CHUNK_RADIUS;
    g->render_radius = RENDER_CHUNK_RADIUS;
    g->delete_radius = DELETE_CHUNK_RADIUS;

    // INITIALIZE WORKER THREADS
    for (int i = 0; i < WORKERS; i++) {
        Worker *worker = g->workers + i;
        worker->index = i;
        worker->state = WORKER_IDLE;
        mtx_init(&worker->mtx, mtx_plain);
        cnd_init(&worker->cnd);
        thrd_create(&worker->thrd, worker_run, worker);
    }

    // OUTER LOOP //
    int running = 1;
    while (running) {
        // DATABASE INITIALIZATION //
        if ( USE_CACHE) {
            db_enable();
            if (db_init(DB_PATH)) {
                return -1;
           }
        }

        // LOCAL VARIABLES //
        reset_model();
        double last_commit = glfwGetTime();

        State *s = &g->state;

        // LOAD STATE FROM DATABASE //
        int loaded = db_load_state(&s->x, &s->y, &s->z, &s->rx, &s->ry);
        if (!loaded) {
            s->y = 180;
        }

        // BEGIN MAIN LOOP //
        double previous = glfwGetTime();
        while (1) {
            // WINDOW SIZE AND SCALE //
            g->scale = get_scale_factor();
            glfwGetFramebufferSize(g->window, &g->width, &g->height);
            glViewport(0, 0, g->width, g->height);

            double now = glfwGetTime();
            double dt = now - previous;
            dt = std::min(dt, 0.2);
            dt = std::max(dt, 0.0);
            previous = now;

            // HANDLE MOUSE INPUT //
            handle_mouse_input();

            // HANDLE MOVEMENT //
            handle_movement(dt);

            // FLUSH DATABASE //
            if (now - last_commit > COMMIT_INTERVAL) {
                last_commit = now;
                db_commit();
            }
            

            // RENDER 3-D SCENE //
            glClear(GL_COLOR_BUFFER_BIT);
            glClear(GL_DEPTH_BUFFER_BIT);
            render_sky(&sky_attrib);
            glClear(GL_DEPTH_BUFFER_BIT);
            render_chunks(&block_attrib);
            

            // RENDER HUD //
            glClear(GL_DEPTH_BUFFER_BIT);
            if (SHOW_CROSSHAIRS) {
                render_crosshairs(&line_attrib);
            }
            if (SHOW_ITEM) {
                render_item(&block_attrib);
            }

            glfwSwapBuffers(g->window);
            glfwPollEvents();
            if (glfwWindowShouldClose(g->window)) {
                running = 0;
                break;
            }
        }

        // SHUTDOWN //
        db_save_state(s->x, s->y, s->z, s->rx, s->ry);
        db_close();
        db_disable();
        delete_all_chunks();
    }

    glfwTerminate();
    return 0;
}
