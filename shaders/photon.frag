#version 460

#define SHELL_STEP 100.0

out vec4 frag_color;

layout(std430, binding=0) readonly buffer ssbo {
    uint shells;
    float heats[];
} sparams;

float circle(vec2 offset, float radius, float dim) {
    return smoothstep(radius * (1.0 - dim), radius, length(offset));
}

void main() {
    vec2 uv = gl_FragCoord.xy / vec2(800.0);

    float dr = length(uv - vec2(0.5));

    uint heat_id = uint(dr * SHELL_STEP);
    float heat = sparams.heats[heat_id];

    vec3 color = mix(vec3(heat / 50.0, 0.0, 0.0), vec3(0.0), circle(uv - vec2(0.5), 0.25, 0.2));

    frag_color = vec4(color, 1.0);
}
