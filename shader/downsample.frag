#version 440

in vec2 uv;

uniform sampler2D color_buffer;
uniform vec2 res;

out vec4 out_Color;

void main()
{
    vec2 color_texel = 1.0 / res;
    float x = color_texel.x;
    float y = color_texel.y;

    vec3 a = texture(color_buffer, vec2(uv.x - 2*x, uv.y + 2*y)).rgb;
    vec3 b = texture(color_buffer, vec2(uv.x,       uv.y + 2*y)).rgb;
    vec3 c = texture(color_buffer, vec2(uv.x + 2*x, uv.y + 2*y)).rgb;

    vec3 d = texture(color_buffer, vec2(uv.x - 2*x, uv.y)).rgb;
    vec3 e = texture(color_buffer, vec2(uv.x,       uv.y)).rgb;
    vec3 f = texture(color_buffer, vec2(uv.x + 2*x, uv.y)).rgb;

    vec3 g = texture(color_buffer, vec2(uv.x - 2*x, uv.y - 2*y)).rgb;
    vec3 h = texture(color_buffer, vec2(uv.x,       uv.y - 2*y)).rgb;
    vec3 i = texture(color_buffer, vec2(uv.x + 2*x, uv.y - 2*y)).rgb;

    vec3 j = texture(color_buffer, vec2(uv.x - x, uv.y + y)).rgb;
    vec3 k = texture(color_buffer, vec2(uv.x + x, uv.y + y)).rgb;
    vec3 l = texture(color_buffer, vec2(uv.x - x, uv.y - y)).rgb;
    vec3 m = texture(color_buffer, vec2(uv.x + x, uv.y - y)).rgb;

    out_Color = vec4(e * 0.125, 1);
    out_Color.rgb += (a + c + g + i) * 0.03125;
    out_Color.rgb += ( b + d + f + h ) * 0.0625;
    out_Color.rgb  += (j + k + l + m) * 0.125;
}
