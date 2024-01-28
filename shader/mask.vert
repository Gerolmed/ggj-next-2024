#version 440

layout(location = 0) in vec2 aPos;

uniform mat4 proj;

void main() {
    gl_Position = proj * vec4(aPos, 0, 1);
}
