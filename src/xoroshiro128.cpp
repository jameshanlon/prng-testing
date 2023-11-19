#include <cstdint>
#include "util.hpp"

//#define SHIFT_A (55)
//#define SHIFT_B (14)
//#define SHIFT_C (36)

#define NAME "xoroshiro128" TOSTRING(SUFFIX)
#define NEXT CONCAT(next_xoroshiro128, SUFFIX)

// NOTE: the parameters (a=24, b=16, b=37) of this version give slightly
//   better results in our test than the 2016 version (a=55, b=14, c=36).
//#define SHIFT_A (24)
//#define SHIFT_B (16)
//#define SHIFT_C (37)

// See http://prng.di.unimi.it/xoroshiro128plus.c

#ifndef LFSR_ITERATIONS
#define LFSR_ITERATIONS (1)
#endif

#ifdef C_LINKAGE
extern "C" const char* generator_name(void);
extern "C" void set_seed(uint64_t s0, uint64_t s1);
extern "C" uint64_t rand64(void);
extern "C" uint32_t rand32(void);
#endif

std::uint64_t count;
std::uint64_t s0, s1, res;

static inline std::uint64_t plus(std::uint64_t s0, std::uint64_t s1) {
  return s0 + s1;
}

static inline std::uint64_t next_xoroshiro128plus(void) {
  // Result returned
  std::uint64_t res = plus(s0, s1);
  // State update
  std::uint64_t sx = s1 ^ s0;
  s0               = rotl(s0, SHIFT_A) ^ sx ^ (sx << SHIFT_B);
  s1               = rotl(sx, SHIFT_C);
  return res;
}

static inline std::uint64_t next_xoroshiro128aox(void) {
  // Result returned
  std::uint64_t res = aox(s0, s1);
  // State update
  std::uint64_t sx = s0 ^ s1;
  s0 = rotl(s0, SHIFT_A) ^ sx ^ (sx << SHIFT_B);
  s1 =  rotl(sx, SHIFT_C);
  return res;
}

const char *generator_name(void) {
  return NAME;
}

void set_seed(std::uint64_t seed0, std::uint64_t seed1) {
  count = 0;
  s0 = seed0;
  s1 = seed1;
}

std::uint32_t rand32() {
  if (count++ % 2 == 0) {
    for (int i = 0; i < LFSR_ITERATIONS; i++) {
      res = NEXT();
    }
    return (uint32_t)(res & 0xFFFFFFFFULL);
  }
  return (uint32_t)((res >> 32) & 0xFFFFFFFFULL);
}

std::uint64_t rand64() {
  std::uint64_t res;
  for (int i = 0; i < LFSR_ITERATIONS; i++) {
    res = NEXT();
  }
  return res;
}
