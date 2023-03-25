#define _XOPEN_SOURCE 500  // M_PI

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "params.h"
#include "photon.h"
#include "wtime.h"

typedef unsigned long ulong;

// global state, heat and heat square in each shell
static float heat[SHELLS];
static float heat2[SHELLS];

void usage(const char *prog_name) {
    fprintf(stderr,
            "usage: %s <SHELLS> <photons> <MU_A> <MU_S> <MICRONS_PER_SHELL> "
            "<SEED>\n",
            prog_name);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc < 7) {
        usage(argv[0]);
    }

    const ulong shells = strtoul(argv[1], NULL, 10);
    const ulong photons = strtoul(argv[2], NULL, 10);
    const ulong seed = strtoul(argv[6], NULL, 10);
    const ulong microns_per_shell = strtoul(argv[5], NULL, 10);

    const float mu_a = strtof(argv[3], NULL);
    const float mu_s = strtof(argv[4], NULL);

    srand(seed);

    const struct photon_params params = {
        .microns_per_shell = microns_per_shell,
        .shells = shells,
        .mu_a = mu_a,
        .mu_s = mu_s,
    };

    double start = wtime();
    for (unsigned int i = 0; i < photons; ++i) {
        photon(params, heat, heat2);
    }
    double end = wtime();

    assert(start <= end);

    double elapsed = end - start;

    printf("# %lf seconds\n", elapsed);
    printf("# %lf K photons per second\n", 1e-3 * photons / elapsed);

    printf("# Radius\tHeat\n");
    printf("# [microns]\t[W/cm^3]\tError\n");

    float t = 4.0f * M_PI * powf(MICRONS_PER_SHELL, 3.0f) * photons / 1e12;

    for (unsigned int i = 0; i < SHELLS - 1; ++i) {
        printf("%6.0f\t%12.5f\t%12.5f\n", i * (float)MICRONS_PER_SHELL,
               heat[i] / t / (i * i + i + 1.0 / 3.0),
               sqrt(heat2[i] - heat[i] * heat[i] / photons) / t /
                   (i * i + i + 1.0f / 3.0f));
    }

    printf("# extra\t%12.5f\n", heat[SHELLS - 1] / photons);

    return 0;
}
