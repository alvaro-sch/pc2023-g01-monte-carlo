#pragma once

struct photon_params {
    float mu_a, mu_s;
    unsigned shells, microns_per_shell;
};

struct shell_heat {
    float heat, heat_squared;
};

void photon(const struct photon_params params, struct shell_heat *heats);
