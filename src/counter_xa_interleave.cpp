#include "pcg_random.hpp"
#include "util.hpp"
#include <cstddef>
#include <cstdint>
#include <random>
#include <set>
extern "C" {
#include "jump.h"
}
#ifdef LINK_C
#include "prngs_hwd.c"
#endif
#include "interleave_seeding.hpp"

#define NAME "counter" TOSTRING(SUFFIX)
#define NEXT CONCAT(next_counter_, SUFFIX)

/// 1000 independent generators whose outputs are interleaved to produce a
/// single generator. This is to test for inter-stream correleation.

uint64_t WARMUP_ITERATIONS = 12;

uint64_t s0[NUM_GENERATORS];
uint64_t s1[NUM_GENERATORS];

uint64_t count;
uint64_t generatorIndex;
uint64_t res;

static inline std::pair<uint64_t, uint64_t> xoroshiro128() {
  uint64_t sx = s1[generatorIndex] ^ s0[generatorIndex];
  uint64_t result_s0 = rotl(s0[generatorIndex], 41) ^ sx ^ (sx << 14);
  uint64_t result_s1 = rotl(sx, 23) ^ rotl(sx, 8) ^ rotl(sx, 61);
  return std::make_pair(result_s0, result_s1);
}

static inline void stateUpdate() {
  __uint128_t counter = static_cast<__uint128_t>(s1[generatorIndex]) << 64 |
                        static_cast<__uint128_t>(s0[generatorIndex]);
  counter += 1;
  s0[generatorIndex] = static_cast<uint64_t>(counter);
  s1[generatorIndex] = static_cast<uint64_t>(counter >> 64);
}

static inline std::uint64_t next_counter_xa(void) {
  // Output post-processing
  std::tie(s0[generatorIndex], s1[generatorIndex]) = xoroshiro128();
  std::uint64_t res = aox(s0[generatorIndex], s1[generatorIndex]);
  // State update
  stateUpdate();
  return res;
}

const char *generator_name(void) { return NAME; }

void set_seed(std::uint64_t seed0, std::uint64_t seed1) {
#if defined SEED_PCG64
  set_seed_pcg64(s0, s1);
#elif defined SEED_MT64
  set_seed_mt64(s0, s1);
#elif defined SEED_EQUIA
  set_seed_equia(s0, s1);
#elif defined SEED_EQUIB
  set_seed_equib(s0, s1);
#elif defined SEED_EQUIC
  set_seed_equic(s0, s1);
#elif defined SEED_EQUID
  set_seed_equid(s0, s1);
#elif defined SEED_EQUIE
  set_seed_equie(s0, s1);
#else
#error No seed scheme defined
#endif
  count = 0;
  generatorIndex = 0;
}

void set_output_shift(size_t shift) {}

uint32_t rand32() {
  if (count++ % 2 == 0) {
    res = NEXT();
    generatorIndex = (generatorIndex + 1) % NUM_GENERATORS;
    return (uint32_t)(res & 0xFFFFFFFFULL);
  }
  return (uint32_t)((res >> 32) & 0xFFFFFFFFULL);
}

uint64_t rand64() {
  uint64_t res;
  res = NEXT();
  generatorIndex = (generatorIndex + 1) % NUM_GENERATORS;
  return res;
}
