#pragma once

struct photon_params {
    float mu_a, mu_s;
    unsigned shells, microns_per_shell;
};

void photon(const struct photon_params params, float *heats, float *heats_squared);
