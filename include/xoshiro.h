#pragma once

#include <stdint.h>

#define XOSHIRO_RAND_MAX UINT32_MAX

void xoshiro_set_seed(uint32_t seed[4]);

uint32_t xoshiro_next_u32();

float xoshiro_next_f32();
