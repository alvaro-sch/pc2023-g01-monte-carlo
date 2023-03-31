#include "photon.h"

#include <math.h>
#include <stdlib.h>

#include "xoshiro.h"

void photon(const struct photon_params params, float *heats,
            float *heats_squared) {
    const float albedo = params.mu_s / (params.mu_s + params.mu_a);
    const float shells_per_mfp =
        1e4 / params.microns_per_shell / (params.mu_a + params.mu_s);

    /* launch */
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float u = 0.0f;
    float v = 0.0f;
    float w = 1.0f;
    float weight = 1.0f;

    for (;;) {
        float t = -logf(rand() / (float)RAND_MAX); /* move */
        //float t = -logf(xoshiro_next_u32() / (float) XOSHIRO_RAND_MAX); /* move */
        //float t = -logf(xoshiro_next_f32()); /* move */
        x += t * u;
        y += t * v;
        z += t * w;

        unsigned int shell =
            sqrtf(x * x + y * y + z * z) * shells_per_mfp; /* absorb */

        if (shell > params.shells - 1) {
            shell = params.shells - 1;
        }

        heats[shell] += (1.0f - albedo) * weight;
        heats_squared[shell] += (1.0f - albedo) * (1.0f - albedo) * weight *
                                weight; /* add up squares */
        weight *= albedo;

        /* New direction, rejection method */
        float xi1, xi2;
        do {
            xi1 = 2.0f * rand() / (float)RAND_MAX - 1.0f;
            xi2 = 2.0f * rand() / (float)RAND_MAX - 1.0f;
            //xi1 = 2.0f * xoshiro_next_u32() / (float) XOSHIRO_RAND_MAX - 1.0f;
            //xi2 = 2.0f * xoshiro_next_u32() / (float) XOSHIRO_RAND_MAX - 1.0f;
            //xi1 = 2.0f * xoshiro_next_f32() - 1.0f;
            //xi2 = 2.0f * xoshiro_next_f32() - 1.0f;
            t = xi1 * xi1 + xi2 * xi2;
        } while (1.0f < t);

        u = 2.0f * t - 1.0f;
        v = xi1 * sqrtf((1.0f - u * u) / t);
        w = xi2 * sqrtf((1.0f - u * u) / t);

        if (weight < 0.001f) { /* roulette */
            if (rand() / (float)RAND_MAX > 0.1f) break;
            //if (xoshiro_next_u32() / (float) XOSHIRO_RAND_MAX > 0.1f) break;
            //if (xoshiro_next_f32() > 0.1f) break;
            weight /= 0.1f;
        }
    }
}
