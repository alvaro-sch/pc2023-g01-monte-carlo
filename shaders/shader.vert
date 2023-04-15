#version 460

vec2 vertices[4] = {
    {-1.0,  1.0},
    {-1.0, -1.0},
    { 1.0, -1.0},
    { 1.0,  1.0}
};

uint indices[6] = {0, 1, 2, 0, 2, 3};

void main() {
    gl_Position = vec4(vertices[indices[gl_VertexID]], 0.0, 1.0);
}
