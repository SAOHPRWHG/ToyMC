#version 330 core

uniform mat4 matrix;
uniform vec3 camera;
uniform float fog_distance;
uniform int ortho;


layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 uv;


out vec2 fragment_uv;
out float fragment_ao;
out float fragment_light;
out float fuzzy_factor;
out float fuzzy_height;
out float diffuse;

const float pi = 3.14159265;
const vec3 light_direction = normalize(vec3(-1.0, 1.0, -1.0));

void main() {
    gl_Position = matrix * position;
    fragment_uv = uv.xy;
    fragment_ao = 0.3 + (1.0 - uv.z) * 0.7;
    fragment_light = uv.w;
    diffuse = max(0.0, dot(normal, light_direction));
    if (bool(ortho)) {
        fuzzy_factor = 0.0;
        fuzzy_height = 0.0;
    }
    else {
        float camera_distance = distance(camera, vec3(position));
        fuzzy_factor = pow(clamp(camera_distance / fog_distance, 0.0, 1.0), 4.0);
        float dy = position.y - camera.y;
        float dx = distance(position.xz, camera.xz);
        fuzzy_height = (atan(dy, dx) + pi / 2) / pi;
    }
}
