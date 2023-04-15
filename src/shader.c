#include "shader.h"

#include <stdio.h>
#include <GL/glew.h>

#define INFO_LOG_LEN 2048

GLuint compile_shader(struct shader_params params) {
    GLuint shader = glCreateShader(params.type);

    glShaderSource(shader, 1, &params.src, &params.length);

    glCompileShader(shader);

    GLint compile_status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);

    if (!compile_status) {
        char info_log[INFO_LOG_LEN];
        glGetShaderInfoLog(shader, INFO_LOG_LEN, NULL, info_log);
        fprintf(stderr, "error compiling shader!\n%s\n", info_log);
        exit(1);
    }

    return shader;
}

GLuint create_program(size_t shader_count, struct shader_params *shader_params) {
    GLuint *shaders = calloc(shader_count, sizeof(*shaders));

    for (size_t i = 0; i < shader_count; ++i) {
        shaders[i] = compile_shader(shader_params[i]);
    }

    GLuint program = glCreateProgram();
    for (size_t i = 0; i < shader_count; ++i) {
        glAttachShader(program, shaders[i]);
    }

    glLinkProgram(program);

    for (size_t i = 0; i < shader_count; ++i) {
        glDeleteShader(shaders[i]);
    }
    free(shaders);

    GLint link_status;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);

    if (!link_status) {
        char info_log[INFO_LOG_LEN];
        glGetProgramInfoLog(program, INFO_LOG_LEN, NULL, info_log);
        fprintf(stderr, "error compiling shader!\n%s\n", info_log);
        exit(1);
    }

    return program;
}
