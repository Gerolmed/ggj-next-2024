#version 440

in vec2 uv;

uniform sampler2D color_buffer;
uniform sampler2D bloom_buffer;
uniform sampler2D mask_buffer;

out vec4 out_Color;

void main() {
    float gamma = 1.3;
    float saturation = 0.8;
    float contrast = 1;
    float bloom = 0.05;
    float exposure = 1.0;
    
    // Dont comment this out or everything burns
    float mask = texture(mask_buffer, uv).r;

    vec3 hdr_color = texture(color_buffer, uv).rgb;

    hdr_color += mask * vec3(0.3, 0.05, 0.05);
    hdr_color += bloom * texture(bloom_buffer, uv).rgb;
    hdr_color = contrast * (hdr_color - 0.5) + vec3(0.5);

    float luminance = dot(hdr_color, vec3(0.299, 0.587, 0.114));
    hdr_color = saturation * hdr_color + (1 - saturation) * vec3(luminance);

    // vec3 color = hdr_color / (hdr_color + vec3(1));
    vec3 color = vec3(1) - exp(-hdr_color * exposure);
    // vec3 color = hdr_color;

    color = pow(color, vec3(gamma));

    out_Color = vec4(color, 1);
}
