#pragma once

#include <stdlib.h>

#include <GL/glew.h>

extern const char _binary_shaders_shader_vert_start[];
extern const char _binary_shaders_shader_vert_end[];

extern char _binary_shaders_photon_frag_start[];
extern char _binary_shaders_photon_frag_end[];

#define SHADER_VERT_OBJ _binary_shaders_shader_vert_start
#define SHADER_VERT_SIZE ((size_t) (_binary_shaders_shader_vert_end - _binary_shaders_shader_vert_start))

#define SHADER_FRAG_OBJ _binary_shaders_photon_frag_start
#define SHADER_FRAG_SIZE ((size_t) (_binary_shaders_photon_frag_end - _binary_shaders_photon_frag_start))

struct shader_params {
    GLenum type;
    GLsizei length;
    const char *src;
};

GLuint create_program(size_t shader_count, struct shader_params *shaders);
