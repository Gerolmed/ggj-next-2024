#version 440

in vec2 uv;

uniform sampler2D color_buffer;
// uniform float filterRadius;

out vec4 out_Color;

void main()
{
    float filter_radius = 0.001;

    float x = filter_radius;
    float y = filter_radius;

    vec3 a = texture(color_buffer, vec2(uv.x - x, uv.y + y)).rgb;
    vec3 b = texture(color_buffer, vec2(uv.x,     uv.y + y)).rgb;
    vec3 c = texture(color_buffer, vec2(uv.x + x, uv.y + y)).rgb;

    vec3 d = texture(color_buffer, vec2(uv.x - x, uv.y)).rgb;
    vec3 e = texture(color_buffer, vec2(uv.x,     uv.y)).rgb;
    vec3 f = texture(color_buffer, vec2(uv.x + x, uv.y)).rgb;

    vec3 g = texture(color_buffer, vec2(uv.x - x, uv.y - y)).rgb;
    vec3 h = texture(color_buffer, vec2(uv.x,     uv.y - y)).rgb;
    vec3 i = texture(color_buffer, vec2(uv.x + x, uv.y - y)).rgb;

    out_Color = vec4(e * 4.0, 0.5);
    out_Color.rgb += (b + d + f + h) * 2.0;
    out_Color.rgb += (a + c + g + i);
    out_Color.rgb *= 1.0 / 16.0;
}
