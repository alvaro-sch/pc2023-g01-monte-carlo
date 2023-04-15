#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "photon.h"
#include "shader.h"

#define SHELL_COUNT 101
#define PHOTON_CAP  1<<16

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_Window *window = SDL_CreateWindow(
        "dynamic photon transfer", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED, 800, 800,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);

    SDL_GLContext context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, context);
    glewInit();

    GLuint shader = create_program(2, (struct shader_params[2]) {
        (struct shader_params) {
            .type = GL_VERTEX_SHADER,
            .length = SHADER_VERT_SIZE,
            .src = SHADER_VERT_OBJ,
        },
        (struct shader_params) {
            .type = GL_FRAGMENT_SHADER,
            .length = SHADER_FRAG_SIZE,
            .src = SHADER_FRAG_OBJ,
        }
    });
    glUseProgram(shader);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    SDL_ShowWindow(window);

    struct photon_params params = {
        .mu_a = 2.0,
        .mu_s = 20.0,
        .shells = SHELL_COUNT,
        .microns_per_shell = 50,
    };

    float *heats = calloc(SHELL_COUNT, sizeof(*heats));
    float *_heats2 = calloc(SHELL_COUNT, sizeof(*_heats2)); // not really used, but needed

    unsigned shells = SHELL_COUNT;
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(shells) + SHELL_COUNT * sizeof(*heats), NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(shells), &shells);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(shells), SHELL_COUNT * sizeof(*heats), heats);

    int remaining_photons = PHOTON_CAP;
    bool should_quit = false;

    while (!should_quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                should_quit = true;
                break;

            case SDL_KEYDOWN:
                switch(event.key.keysym.scancode) {
                case SDL_SCANCODE_R:
                    memset(heats, 0, SHELL_COUNT * sizeof(*heats));
                    remaining_photons = PHOTON_CAP;
                    break;

                default:
                    break;
                }
                break;

            default:
                break;
            }
        }

        if (remaining_photons > 0) {
            --remaining_photons;
            photon(params, heats, _heats2);
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(shells), SHELL_COUNT * sizeof(*heats), heats);
        }

        // printf("%f\n", heats[0]);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        SDL_GL_SwapWindow(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
}
