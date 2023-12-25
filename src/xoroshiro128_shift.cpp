#include <cstddef>
#include <cstdint>
#include "util.hpp"

#ifdef LINK_C
#include "prngs_hwd.c"
#endif

#define NAME "xoroshiro128" TOSTRING(SUFFIX)
#define NEXT CONCAT(next_xoroshiro128, SUFFIX)

uint64_t count;
uint64_t res;
uint64_t s0, s1;
size_t outputShift = 0;

static inline uint64_t plus(uint64_t s0, uint64_t s1) {
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

void set_output_shift(size_t shift) {
  outputShift = shift;
}

std::uint32_t rand32() {
  // Always return the next value since we are using the shift to access
  // individual bits in the output for testing.
  res = NEXT() >> outputShift;
  return (uint32_t)(res & 0xFFFFFFFFULL);
}

std::uint64_t rand64() {
  std::uint64_t res;
  res = NEXT() >> outputShift;
  return res;
}
