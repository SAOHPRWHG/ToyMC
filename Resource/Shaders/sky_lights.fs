#version 330 core

uniform sampler2D sampler;
uniform float timer;
out vec4 FragColor;

in vec4 fragment_uv;

void main() {
    vec2 uv = vec2(timer, fragment_uv.t);
    FragColor = texture2D(sampler, uv);
}
