#define _XOPEN_SOURCE 500  // M_PI

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#include "params.h"
#include "photon.h"
#include "wtime.h"
#include "xoshiro.h"

typedef unsigned long ulong;

void usage(const char *prog_name) {
    fprintf(stderr, "usage: %s <photons> <SEED>\n", prog_name);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        usage(argv[0]);
    }

    ulong photons = strtoul(argv[1], NULL, 10);
    ulong seed = strtoul(argv[2], NULL, 10);

    srand(seed);

    const struct photon_params params = {
        .microns_per_shell = MICRONS_PER_SHELL,
        .shells = SHELLS,
        .mu_a = MU_A,
        .mu_s = MU_S,
    };

    float heats[SHELLS] = { 0.0 };
    float heats2[SHELLS] = { 0.0 };

    uint32_t rand_state[4] = { 0, rand(), rand(), rand() };

    double start = wtime();
    #pragma omp parallel default(none) \
        shared(params, photons) \
        private(rand_state) \
        reduction(+:heats,heats2)
    {
        // rand_state[0] = omp_get_thread_num();

        #pragma omp for
        for (unsigned int i = 0; i < photons; ++i) {
            photon(params, heats, heats2, rand_state);
        }
    }
    double end = wtime();

    assert(start <= end);

    double elapsed = end - start;

    printf("%lf\n", elapsed);
    printf("# %lf K photons per second\n", 1e-3 * photons / elapsed);

    printf("# Radius\tHeat\n");
    printf("# [microns]\t[W/cm^3]\tError\n");

    float t = 4.0f * M_PI * powf(MICRONS_PER_SHELL, 3.0f) * photons / 1e12;

    for (unsigned int i = 0; i < SHELLS - 1; ++i) {
        float vol = i * i + i + 1.0 / 3.0;

        printf("%6.0f\t%12.5f\t%12.5f\n", i * (float)MICRONS_PER_SHELL,
               heats[i] / t / vol,
               sqrt(heats2[i] -
                    heats[i] * heats[i] / photons) /
                   t / vol);
    }

    printf("# extra\t%12.5f\n", heats[SHELLS - 1] / photons);

    return 0;
}
