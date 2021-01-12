#version 330 core

uniform mat4 matrix;

layout (location = 0) in vec4 position;

void main() {
    gl_Position = matrix * position;
}
