#include <cstddef>
#include <cstdint>
#include <utility>
#include <tuple>
#include "util.hpp"

#define NAME "counter_" TOSTRING(SUFFIX)
#define NEXT CONCAT(next_counter_, SUFFIX)

std::uint64_t count;
std::uint64_t s0, s1, res; // s0 treated as low bits, s1 as high bits.

static inline void stateUpdate() {
  __uint128_t counter = static_cast<__uint128_t>(s1)<<64 | static_cast<__uint128_t>(s0);
  counter += 1;
  s0 = static_cast<uint64_t>(counter);
  s1 = static_cast<uint64_t>(counter >> 64);
}

static inline std::pair<uint64_t, uint64_t> grayMapping() {
  // Create a 128 bit state variable.
  __uint128_t state = static_cast<__uint128_t>(s1)<<64 | static_cast<__uint128_t>(s0);
  // Gray code the counter by XORing adjacent bits Gn = Cn+1 XOR Cn (for n=0 to 126) G127 = C127.
  state ^= ((state << 127) | (state >> 1));
  // Swap G[127:64] with G[63:0] using bit C0 (every other cycle).
  uint64_t hiPart = static_cast<uint64_t>(state >> 64);
  uint64_t loPart = static_cast<uint64_t>(state);
  if (loPart & 1ULL) {
    return std::make_pair(hiPart, loPart);
  } else {
    return std::make_pair(loPart, hiPart);
  }
}

static inline std::pair<uint64_t, uint64_t> xoroshiro128() {
  uint64_t sx = s1 ^ s0;
  uint64_t result_s0 = rotl(s0, 41) ^ sx ^ (sx << 14);
  uint64_t result_s1 = rotl(sx, 23) ^ rotl(sx, 8) ^ rotl(sx, 61);
  return std::make_pair(result_s0, result_s1);
}

static inline std::uint64_t next_counter_xa(void) {
  // Output post-processing
  uint64_t tmp0, tmp1;
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::uint64_t res = aox(tmp0, tmp1);
  // State update
  stateUpdate();
  return res;
}

static inline std::uint64_t next_counter_2xa(void) {
  // Output post-processing
  uint64_t tmp0, tmp1;
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::uint64_t res = aox(tmp0, tmp1);
  // State update
  stateUpdate();
  return res;
}

static inline std::uint64_t next_counter_3xa(void) {
  // Output post-processing
  uint64_t tmp0, tmp1;
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::uint64_t res = aox(tmp0, tmp1);
  // State update
  stateUpdate();
  return res;
}

static inline std::uint64_t next_counter_5xa(void) {
  // Output post-processing
  uint64_t tmp0, tmp1;
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::uint64_t res = aox(tmp0, tmp1);
  // State update
  stateUpdate();
  return res;
}

static inline std::uint64_t next_counter_x(void) {
  // Output post-processing
  uint64_t tmp0, tmp1;
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::uint64_t res = tmp0;
  // State update
  stateUpdate();
  return res;
}

static inline std::uint64_t next_counter_2x(void) {
  // Output post-processing
  uint64_t tmp0, tmp1;
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::uint64_t res = tmp0;
  // State update
  stateUpdate();
  return res;
}

static inline std::uint64_t next_counter_3x(void) {
  // Output post-processing
  uint64_t tmp0, tmp1;
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::uint64_t res = tmp0;
  // State update
  stateUpdate();
  return res;
}

static inline std::uint64_t next_counter_5x(void) {
  // Output post-processing
  uint64_t tmp0, tmp1;
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::uint64_t res = tmp0;
  // State update
  stateUpdate();
  return res;
}

static inline std::uint64_t next_counter_12x(void) {
  // Output post-processing
  uint64_t tmp0, tmp1;
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::uint64_t res = tmp0;
  // State update
  stateUpdate();
  return res;
}

static inline std::uint64_t next_counter_gray_12x(void) {
  // Output post-processing
  uint64_t tmp0, tmp1;
  std::tie(tmp0, tmp1) = grayMapping();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::tie(tmp0, tmp1) = xoroshiro128();
  std::uint64_t res = tmp0;
  // State update
  stateUpdate();
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

void set_output_shift(size_t shift) {}

std::uint32_t rand32() {
  if (count++ % 2 == 0) {
    res = NEXT();
    return (uint32_t)(res & 0xFFFFFFFFULL);
  }
  return (uint32_t)((res >> 32) & 0xFFFFFFFFULL);
}

std::uint64_t rand64() {
  std::uint64_t res;
  res = NEXT();
  return res;
}
