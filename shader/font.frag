#version 440

in vec2 uv;
in vec3 color;

uniform sampler2D base_color;

out vec4 out_Color;

void main() {
    float value = texture(base_color, uv).r;
    if (value < 0.1) {
        discard;
    }
    out_Color = vec4(color * value, 1);
}
