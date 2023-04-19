#version 460

#define SHELL_STEP 140.0

out vec4 frag_color;

layout(location = 0) uniform ivec2 screen;

layout(std430, binding = 0) readonly buffer ssbo {
    float heats[];
} sparams;

float circle(vec2 offset, float radius, float dim) {
    return smoothstep(radius * (1.0 - dim), radius, length(offset));
}

void main() {
    vec2 uv = gl_FragCoord.xy / vec2(screen);

    float dr = length(uv - vec2(0.5));

    int heat_id = clamp(int(dr * SHELL_STEP), 0, sparams.heats.length());
    float heat = sparams.heats[heat_id];

    //vec3 color = mix(vec3(heat / 50.0, 0.0, 0.0), vec3(0.0), circle(uv - vec2(0.5), 0.25, 0.2));
    vec3 color = vec3(heat / 100.0, 0.0, 0.0);

    frag_color = vec4(color, 1.0);
}
