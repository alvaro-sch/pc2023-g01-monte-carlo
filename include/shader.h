#pragma once

#include <stdlib.h>

#include <GL/glew.h>

// these shaders are linked from an object file
// their contents can be read with `objdump -t -s <shader_file>`
extern const char _binary_shaders_shader_vert_start[];
extern const char _binary_shaders_shader_vert_end[];

extern const char _binary_shaders_photon_frag_start[];
extern const char _binary_shaders_photon_frag_end[];

#define SHADER_VERT_OBJ _binary_shaders_shader_vert_start
#define SHADER_VERT_SIZE ((size_t) (_binary_shaders_shader_vert_end - _binary_shaders_shader_vert_start))

#define SHADER_FRAG_OBJ _binary_shaders_photon_frag_start
#define SHADER_FRAG_SIZE ((size_t) (_binary_shaders_photon_frag_end - _binary_shaders_photon_frag_start))

struct shader_params {
    GLenum type;
    GLsizei length;
    const char *src;
};

GLuint create_program(size_t shader_count, const struct shader_params *shaders);
