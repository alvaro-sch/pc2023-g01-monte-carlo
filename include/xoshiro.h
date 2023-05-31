#pragma once

#include <stdint.h>

#define XOSHIRO_RAND_MAX UINT32_MAX

uint32_t xoshiro_next_u32(uint32_t s[4]);

float xoshiro_next_f32(uint32_t s[4]);
