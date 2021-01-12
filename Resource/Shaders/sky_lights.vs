#version 330 core

uniform mat4 matrix;

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 uv;

out vec2 fragment_uv;

void main() {
    gl_Position = matrix * position;
    fragment_uv = uv.xy;
}
