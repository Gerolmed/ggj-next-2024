#version 440

layout(location = 0) in vec2 aPos;

out vec2 uv;

void main() {
    uv = (aPos + 1) / 2;
    gl_Position = vec4(aPos, 0, 1);
}
