#version 460

#define SHELL_STEP 250.0

out vec4 frag_color;

layout(location = 0) uniform ivec2 screen;

layout(std430, binding = 0) readonly buffer c_ssbo {
    vec4 color[];
} colormap;

layout(std430, binding = 1) readonly buffer s_ssbo {
    float heats[];
} shells;

void main() {
    vec2 uv = gl_FragCoord.xy / vec2(screen);

    float dr = length(uv - vec2(0.5));

    int heat_id = clamp(int(dr * SHELL_STEP), 0, shells.heats.length() - 1);

    float heat = shells.heats[heat_id];
    float heat_fit = 2.0 / (1.0 + exp(-heat * 0.02)) - 1.0;

    int c_id = int(float(colormap.color.length() - 1) * heat_fit);

    frag_color = colormap.color[c_id];
}
