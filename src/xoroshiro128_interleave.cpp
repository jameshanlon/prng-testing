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

#define NAME "xoroshiro128" TOSTRING(SUFFIX)
#define NEXT CONCAT(next_xoroshiro128, SUFFIX)

/// 1000 independent generators whose outputs are interleaved to produce a
/// single generator. This is to test for inter-stream correleation.

uint64_t WARMUP_ITERATIONS = 12;

uint64_t s0[NUM_GENERATORS];
uint64_t s1[NUM_GENERATORS];

uint64_t count;
uint64_t generatorIndex;
uint64_t res;

inline uint64_t plus(uint64_t s0, uint64_t s1) { return s0 + s1; }

inline uint64_t next_xoroshiro128plus(void) {
  // Result returned
  uint64_t res = plus(s0[generatorIndex], s1[generatorIndex]);
  // State update
  uint64_t sx = s1[generatorIndex] ^ s0[generatorIndex];
  s0[generatorIndex] = rotl(s0[generatorIndex], SHIFT_A) ^ sx ^ (sx << SHIFT_B);
  s1[generatorIndex] = rotl(sx, SHIFT_C);
  return res;
}

inline uint64_t next_xoroshiro128aox(void) {
  // Result returned
  uint64_t res = aox(s0[generatorIndex], s1[generatorIndex]);
  // State update
  uint64_t sx = s0[generatorIndex] ^ s1[generatorIndex];
  s0[generatorIndex] = rotl(s0[generatorIndex], SHIFT_A) ^ sx ^ (sx << SHIFT_B);
  s1[generatorIndex] = rotl(sx, SHIFT_C);
  return res;
}

const char *generator_name(void) { return NAME; }

void jump() {
#if defined JUMP_SHORT_1000
  // Jump a number of iterations that is 32 TB output divided by 1000.
  static const uint64_t JUMP[] = {0xb0801d23e66fc68e, 0xf68fe69aa025f143};
#elif defined JUMP_SHORT_100
  // Jump a number of iterations that is 32 TB output divided by 100.
  static const uint64_t JUMP[] = {0xe0cfd65cb2619bb1, 0x5dcfdc286adcb15c};
#elif defined JUMP_SHORT_10
  // Jump a number of iterations that is 32 TB output divided by 10.
  static const uint64_t JUMP[] = {0x1dbbc323b29fa11c, 0xf9d148a50cebabbe};
#elif defined JUMP_SHORT_128MB
  // Jump by 128 MB of output.
  static const uint64_t JUMP[] = {0x5c8f6252d9389976, 0xb2414b0892b97e22};
#elif defined JUMP_SHORT_256MB
  // Jump by 256 MB of output.
  static const uint64_t JUMP[] = {0x563b590e7bb99129, 0x1ea7ed43fe3f13ed};
#elif defined JUMP_SHORT_1024MB
  // Jump by 1024 MB of output.
  static const uint64_t JUMP[] = {0xd41a2bed2e66fd67, 0xe9a8a19463e84de7};
#else
  // Jump 2^64 iterations.
  static const uint64_t JUMP[] = {0xdf900294d8f554a5, 0x170865df4b3201fc};
#endif
  uint64_t s0_temp = 0;
  uint64_t s1_temp = 0;
  for (int i = 0; i < 2; i++) {
    for (int b = 0; b < 64; b++) {
      if (JUMP[i] & UINT64_C(1) << b) {
        s0_temp ^= s0[generatorIndex];
        s1_temp ^= s1[generatorIndex];
      }
      next_xoroshiro128aox();
    }
  }
  s0[generatorIndex] = s0_temp;
  s1[generatorIndex] = s1_temp;
}

void jump2(uint64_t JUMP[2]) {
  uint64_t s0_temp = 0;
  uint64_t s1_temp = 0;
  for (int i = 0; i < 2; i++) {
    for (int b = 0; b < 64; b++) {
      if (JUMP[i] & UINT64_C(1) << b) {
        s0_temp ^= s0[generatorIndex];
        s1_temp ^= s1[generatorIndex];
      }
      next_xoroshiro128aox();
    }
  }
  s0[generatorIndex] = s0_temp;
  s1[generatorIndex] = s1_temp;
}

void set_seed(uint64_t seed0, uint64_t seed1) {
#if defined SEED_JUMP
  // Jump polynomial calculation setup.
  int i;
  int x, y, z;
  uint64_t prngs[2];
  uint64_t m[128][2];
  const uint64_t one = 0x0000000000000001;
  const __uint128_t one128 = 0x00000000000000000000000000000001;
  uint64_t cpoly1[2];
  uint64_t cpolyj[2];
  x = SHIFT_A;
  y = SHIFT_B;
  z = SHIFT_C;
  // build a matrix for a single-step.
  for (i = 0; i < 128; i++) {
    prngs[0] = (i >= 64) ? (uint64_t)0 : one << i;
    prngs[1] = (i < 64) ? (uint64_t)0 : one << (i - 64);
    prngupdate_xoro(x, y, z, prngs, &prngs[0]);
    m[i][0] = prngs[0];
    m[i][1] = prngs[1];
  }
  // calcuate characteristic polynomial
  charpoly(m, &cpoly1[0]);
  assert(cpoly1[0] == 0x5fd66762f0e1c001 && cpoly1[1] == 0x00653ced7f29f88a);
  // Sanity check: jump forwards 2^128-1. Should be equivalent to jumping
  // forwards by 0.
  jumppoly(cpoly1, (__uint128_t)(-one128), cpolyj);
  assert(cpolyj[1] == 0 && cpolyj[0] == 1);
  // Seed generators using jump()
  pcg64 generator(PCG_128BIT_CONSTANT(-1, 1));
  for (generatorIndex = 0; generatorIndex < NUM_GENERATORS; generatorIndex++) {
    s0[generatorIndex] = 1;
    s1[generatorIndex] = -1;
    // Create a jump polynomial.
    uint64_t jumpDistanceLo = generator();
    uint64_t jumpDistanceHi = generator();
    __uint128_t jumpDistance =
        ((__uint128_t)jumpDistanceHi << 64) | (__uint128_t)jumpDistanceLo;
    // std::cout << "Seed " << generatorIndex << " jump by " << std::hex <<
    // jumpDistanceHi << " " << jumpDistanceLo << "\n";
    jumppoly(cpoly1, jumpDistance, cpolyj);
    // for (size_t i=0; i<generatorIndex; i++) {
    jump2(cpolyj);
    //}
  }
#elif defined SEED_PCG64
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
  // Check that all seeds are unique.
  std::set<uint64_t> s0set(s0, s0 + NUM_GENERATORS);
  std::set<uint64_t> s1set(s1, s1 + NUM_GENERATORS);
  if (s0set.size() != NUM_GENERATORS) {
    std::cerr << "Duplicate s0 seeds ("
              << std::to_string(NUM_GENERATORS - s0set.size())
              << " duplicates)\n";
    for (auto x : s0set) {
      std::cerr << std::to_string(x) << "\n";
    }
  }
  if (s1set.size() != NUM_GENERATORS) {
    std::cerr << "Duplicate s1 seeds ("
              << std::to_string(NUM_GENERATORS - s1set.size())
              << " duplicates)\n";
    for (auto x : s1set) {
      std::cerr << std::to_string(x) << "\n";
    }
  }
  // Warm up the generators
  for (generatorIndex = 0; generatorIndex < NUM_GENERATORS; generatorIndex++) {
    for (size_t j = 0; j < WARMUP_ITERATIONS; j++) {
      next_xoroshiro128aox();
    }
  }
  // Initialise counters.
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
