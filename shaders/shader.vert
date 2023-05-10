#version 460

#ifndef VULKAN
#define gl_VertexIndex gl_VertexID
#endif

vec2 vertices[4] = {
    {-1.0,  1.0},
    {-1.0, -1.0},
    { 1.0, -1.0},
    { 1.0,  1.0}
};

uint indices[6] = {0, 1, 2, 0, 2, 3};

void main() {
    gl_Position = vec4(vertices[indices[gl_VertexIndex]], 0.0, 1.0);
}
