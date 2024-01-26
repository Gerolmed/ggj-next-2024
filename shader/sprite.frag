#version 440

in vec2 uv;
in vec3 color;

uniform sampler2D base_color;

out vec4 out_Color;

void main() {
    out_Color = texture(base_color, uv);
    out_Color.rgb *= color;
    if (out_Color.a < 0.1) {
        discard;
    }
}
