# Monte Carlo method for photon transport - Lab 1

This document

## Hardware

All the benchmarks were done in the following system:

| OS |
| - |
| Linux atom 6.1.0-5-amd64 #1 SMP PREEMPT_DYNAMIC Debian 6.1.12-1 (2023-02-15) x86_64 GNU/Linux |

| CPU | |
| - | - |
| Model | AMD EPYC 7643 48-Core Processor |
| Arch | x86_64 |
| L1d | 1.5 MiB (48 instances) |
| L1i | 1.5 MiB (48 instances) |
| L2 | 24 MiB (48 instances) |
| L3 | 256 MiB (8 instances) |
| Max Frequency | 3640.9170 MHz |

| CCs |
| - |
| gcc 12.2.0 |
| clang 15.0.6 |
| icx 2023.0.0 |

## Metrics

All metrics related to the simulation itself are taken from the average
runtime of N photons acting on the same medium with a fixed seed value
S, the simulation has other parameters which will be specified, but the
only one that affects the final time is the seed since the exit condition
of a photon has a random factor in it.

Note that this metric is size-independant because it is an average.

## Optimizations

### Compiler flags

The set of flags we decided to use were `-O2 -DNDEBUG -march=native -ffast-math -funroll-loops`.

### prng

The original code used the default C prng which has very few restrictions
concerning performance and randomness, someone initially may think that
it would probably be too fast to care, but some implementations have a few
obscure shenanigans, like [the possibility of calling malloc](https://www.thingsquare.com/blog/articles/rand-may-call-malloc/).

We compared C's prng agains other 2 well known prngs: *splitmix64* and
*xoshiro* doing 10 rounds of generating 10 million numbers, the
results were the following.

| prng | seconds |
| - | - |
| C | 0.0782985 |
| splitmix64 | 0.0265295 |
| xoshiro256** | 0.0249912 |

For being the fastest, we chose to replace C's `rand()` with xoshiro.

The previous isolated benchmarks were done with one specific version
of xoshiro which is made for generating 64 bit random integers.
However, we can do better, for generating floats in the range [0, 1],
instead of dividing the result by the maximum possible number, manually
construct the floating point number with it, like so.

```c
union {
    uint32_t i;
    float f;
} result = { .i = UINT32_C(0x7F) << 23 | x >> 9 };

return result.f - 1.0;
```

That rather confusing code snippet does the following:

1. It creates an IEEE-754 number using the top bits of the random number
*x* as the mantissa, setting sign to 0 and exponent to all 1s, this is
done in a 32 bit integer because that level of bit manipulation cannot be
done in floats.

1. We need that result as a float, but that cannot be done directly
because casting will **convert** the integer to a float, but we actually
want to **interpret** the bits as a float, that's done with the union, it
essentially stores both the int and the float in the same place, we write
it as an int, but read it as a float, just doing that avoids conversion.

1. The result of the previous steps will create a number in [1, 2],
so we just subtract 1 from it to get a random number in [0, 1].

(insertar resultados de original | cambiar prng | usar la técnica cheta)

### fpdiv vs fpmul

It's known that floating point operations in general are slower than
integer ones, even when ignoring edge-cases with infinities and NaNs.

Although it's inevitable to avoid floating point operations, there's one
specific line of code that may produce a small slowdown.

```c
weight /= 0.1f;
```

This line of code essentially multiplies `weight` by 10, but using a
division, which tends to be slower than multiplication in most cases.

Isolating this snippet in godbolt we observed the following code being
generated.

```c
// -O3
    unpcklps    xmm0, xmm0
    cvtps2pd    xmm0, xmm0
    divsd       xmm0, QWORD PTR .LC0[rip]
    unpcklpd    xmm0, xmm0
    cvtpd2ps    xmm0, xmm0

// -O1 -ffast-math
    mulss   xmm0, DWORD PTR .LC0[rip]
```

This shows that the compiler also thinks that floating point division is
slower than multiplication but unless we use `-ffast-math` it does not
replace the division, which must have something to do with the edge cases
previously mentioned.

With that we were almost convinced that the compiler would optimize that
line for us using flags such as `-O3 -ffast-math` but it turned out it
didn't.

```
            weight /= 0.1f;
    15f8:	f3 0f 10 45 d4       	    movss  xmm0,DWORD PTR [rbp-0x2c]
    15fd:	f3 0f 10 0d 2f 0a 00 00 	movss  xmm1,DWORD PTR [rip+0xa2f]        # 2034 <_IO_stdin_used+0x34>
    1605:	f3 0f 5e c1          	 >> divss  xmm0,xmm1 << division spotted
    1609:	f3 0f 11 45 d4       	    movss  DWORD PTR [rbp-0x2c],xmm0
```

So we manually replaced that in the source code. The results were the
following.

(insertar imagen de div vs mult)

## Putting all together

(before vs after con todo)