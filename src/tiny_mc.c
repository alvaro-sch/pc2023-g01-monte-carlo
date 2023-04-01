#define _XOPEN_SOURCE 500  // M_PI

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "params.h"
#include "photon.h"
#include "wtime.h"
#include "xoshiro.h"

typedef unsigned long ulong;

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

    ulong shells = strtoul(argv[1], NULL, 10);
    ulong photons = strtoul(argv[2], NULL, 10);
    ulong seed = strtoul(argv[6], NULL, 10);
    ulong microns_per_shell = strtoul(argv[5], NULL, 10);

    float mu_a = strtof(argv[3], NULL);
    float mu_s = strtof(argv[4], NULL);

    srand(seed);
    xoshiro_set_seed((uint32_t[4]){rand(), rand(), rand(), rand()});

    const struct photon_params params = {
        .microns_per_shell = microns_per_shell,
        .shells = shells,
        .mu_a = mu_a,
        .mu_s = mu_s,
    };

    float *heats = calloc(shells, sizeof(float));
    float *heats_squared = calloc(shells, sizeof(float));

    double start = wtime();
    for (unsigned int i = 0; i < photons; ++i) {
        photon(params, heats, heats_squared);
    }
    double end = wtime();

    assert(start <= end);

    double elapsed = end - start;

    printf("%lf\n", elapsed);
    printf("# %lf K photons per second\n", 1e-3 * photons / elapsed);

    printf("# Radius\tHeat\n");
    printf("# [microns]\t[W/cm^3]\tError\n");

    float t = 4.0f * M_PI * powf(microns_per_shell, 3.0f) * photons / 1e12;

    for (unsigned int i = 0; i < shells - 1; ++i) {
        printf("%6.0f\t%12.5f\t%12.5f\n", i * (float)microns_per_shell,
               heats[i] / t / (i * i + i + 1.0 / 3.0),
               sqrt(heats_squared[i] - heats[i] * heats[i] / photons) / t /
                   (i * i + i + 1.0f / 3.0f));
    }

    printf("# extra\t%12.5f\n", heats[shells - 1] / photons);

    free(heats);
    free(heats_squared);

    return 0;
}
