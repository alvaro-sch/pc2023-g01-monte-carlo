#include "xoshiro.h"

#include <stdint.h>
#include <string.h>

static uint32_t s[4];

static inline uint32_t rotl(const uint32_t x, int k) {
    return (x << k) | (x >> (32 - k));
}

void xoshiro_set_seed(uint32_t seed[4]) {
    s[0] = seed[0];
    s[1] = seed[1];
    s[2] = seed[2];
    s[3] = seed[3];
}

// xoshiro128+ specially optimized for generating floating point numbers
// source https://prng.di.unimi.it/xoshiro128plus.c
float xoshiro_next_f32() {
    uint32_t x = s[0] + s[3];

    const uint32_t t = s[1] << 9;

    s[2] ^= s[0];
    s[3] ^= s[1];
    s[1] ^= s[2];
    s[0] ^= s[3];

    s[2] ^= t;

    s[3] = rotl(s[3], 11);

    x = UINT32_C(0x7F) << 23 | x >> 9;

    float result;
    memcpy(&result, &x, sizeof(result));

    return result - 1.0;
}

// xoshiro128** unused in this simulation, but useful for comparrison vs
// next_f32 source https://prng.di.unimi.it/xoshiro128starstar.c
uint32_t xoshiro_next_u32() {
    const uint32_t result = rotl(s[1] * 5, 7) * 9;

    const uint32_t t = s[1] << 9;

    s[2] ^= s[0];
    s[3] ^= s[1];
    s[1] ^= s[2];
    s[0] ^= s[3];

    s[2] ^= t;

    s[3] = rotl(s[3], 11);

    return result;
}
