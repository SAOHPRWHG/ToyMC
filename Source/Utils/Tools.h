#ifndef _Tool_h_
#define _Tool_h_

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Setting.h"
#include <cmath>

constexpr float PI = 3.14159265359;
//#define DEGREES(radians) ((radians) * 180 / PI)
#define RADIANS(degrees) ((degrees)*PI / 180)
#define ABS(x) ((x) < 0 ? (-(x)) : (x))
#define SIGN(x) (((x) > 0) - ((x) < 0))

inline float to_degree(const float radians)
{
    return radians * 180 / PI;
}

inline float to_radian(const float degrees)
{
    return degrees * PI / 180;
}

GLuint gen_buffer(GLsizei size, GLfloat *data);
void del_buffer(GLuint buffer);
GLfloat *malloc_faces(int components, int faces);
GLuint gen_faces(int components, int faces, GLfloat *data);
GLuint make_shader(GLenum type, const char *source);
GLuint load_shader(GLenum type, const char *path);
GLuint make_program(GLuint shader1, GLuint shader2);
GLuint load_program(const char *path1, const char *path2);
void load_png_texture(const char *file_name);
char *tokenize(char *str, const char *delim, char **key);
int char_width(char input);
int string_width(const char *input);
int wrap(const char *input, int max_width, char *output, int max_length);

#endif
