#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "photon.h"
#include "shader.h"

#ifndef MAX_PHOTONS_PER_FRAME
#define MAX_PHOTONS_PER_FRAME 20
#endif

#ifndef TARGET_FPS
#define TARGET_FPS 75
#endif

#define MAX_FRAME_TIME ((double) 1.0 / TARGET_FPS)

#define SHELL_COUNT 300
#define PHOTON_CAP  1<<16

static bool should_quit;
static int width, height;
static int remaining_photons;
static float heats[SHELL_COUNT], _heats2[SHELL_COUNT];

static struct photon_params params = {
    .mu_a = 2.0,
    .mu_s = 20.0,
    .shells = SHELL_COUNT,
    .microns_per_shell = 50,
};

const static float colormap[] = {
    #include "inferno512.cmap"
};

void handle_event(const SDL_Event *event) {
    switch (event->type) {
    case SDL_QUIT:
        should_quit = true;
        break;

    case SDL_WINDOWEVENT:
        switch (event->window.event) {
        case SDL_WINDOWEVENT_RESIZED:
            width = event->window.data1;
            height = event->window.data2;
            glViewport(0, 0, width, height);
            glUniform2i(0, width, height);
            break;

        default:
            break;
        }
        break;

    case SDL_KEYDOWN:
        switch(event->key.keysym.scancode) {
        case SDL_SCANCODE_R:
            memset(heats, 0, sizeof(heats));
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

void update(void) {
    if (remaining_photons <= 0) {
        return;
    }

    int remaining_photons_in_frame = MAX_PHOTONS_PER_FRAME;

    while (remaining_photons > 0 && remaining_photons_in_frame > 0) {
        --remaining_photons;
        --remaining_photons_in_frame;

        photon(params, heats, _heats2);
    }

    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(heats), heats);
}

int main(void) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

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
    SDL_GetWindowSize(window, &width, &height);
    glUniform2i(0, width, height);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    SDL_ShowWindow(window);

    GLuint ssbos[2];
    glGenBuffers(2, ssbos);

    // colormap: written only once
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbos[0]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbos[0]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(colormap), colormap, GL_STATIC_DRAW);

    // heat per shell: written almost every frame
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbos[1]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbos[1]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(heats), heats, GL_DYNAMIC_DRAW);

    should_quit = false;
    remaining_photons = PHOTON_CAP;

    SDL_Event event;
    while (!should_quit) {
        while (SDL_PollEvent(&event)) {
            handle_event(&event);
        }

        update();

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        SDL_GL_SwapWindow(window);
    }

    glDeleteBuffers(2, ssbos);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(shader);

    SDL_GL_DeleteContext(context);

    SDL_DestroyWindow(window);
    SDL_Quit();
}
