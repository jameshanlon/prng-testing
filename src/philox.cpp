#include "util.hpp"
#include <cstddef>
#include <cstdint>
#include <iostream>

#define PHILOX4X32_ROUNDS (10)
#define PHILOX2X64_ROUNDS (10)

#define NAME "philox" TOSTRING(SUFFIX)
#define NEXT CONCAT(next_philox, SUFFIX)

__uint128_t counter;
std::uint64_t reshi, reslo;
std::uint64_t count;
std::uint32_t k0, k1;

static inline std::uint64_t mul32_lo(std::uint64_t a, std::uint64_t b) {
  return (a * b) & 0xFFFFFFFFULL;
}

static inline std::uint64_t mul32_hi(std::uint64_t a, std::uint64_t b) {
  return ((a * b) >> 32) & 0xFFFFFFFFULL;
}

static inline std::uint64_t mul64_lo(std::uint64_t a, std::uint64_t b) {
  __uint128_t a128 = a;
  __uint128_t b128 = b;
  __uint128_t r = a128 * b128;
  return static_cast<std::uint64_t>(r);
}

static inline std::uint64_t mul64_hi(std::uint64_t a, std::uint64_t b) {
  __uint128_t a128 = a;
  __uint128_t b128 = b;
  __uint128_t r = (a128 * b128) >> 64;
  return static_cast<std::uint64_t>(r);
}

static inline void next_philox4x32_10(void) {
  std::uint64_t r0, r1, l0, l1;
  std::uint64_t r0_next, r1_next, l0_next, l1_next;
  r0 = counter & 0xFFFFFFFFULL;
  l0 = (counter >> 32) & 0xFFFFFFFFULL;
  r1 = (counter >> 64) & 0xFFFFFFFFULL;
  l1 = (counter >> 96) & 0xFFFFFFFFULL;
  for (size_t i = 0; i < PHILOX4X32_ROUNDS; ++i) {
    l1_next = mul32_lo(r1, 0xD2511F53ULL);
    r1_next = mul32_hi(r0, 0xCD9E8D57ULL) ^ k0 ^ l0;
    l0_next = mul32_lo(r0, 0xCD9E8D57ULL);
    r0_next = mul32_hi(r1, 0xD2511F53ULL) ^ k1 ^ l1;
    l1 = l1_next;
    l0 = l0_next;
    r1 = r1_next;
    r0 = r0_next;
    k1 += 0xBB67AE85;
    k0 += 0x9E3779B9;
  }
  reshi = (l1 << 32) | (r1 & 0xFFFFFFFFULL);
  reslo = (l0 << 32) | (r0 & 0xFFFFFFFFULL);
  counter++;
#ifdef RESET_KEYS
  // This resetting of the key value is necessary to match
  // with the reference implementation of this generator.
  k0 = 0;
  k1 = 0;
#endif
}

static inline void next_philox2x64_10(void) {
  std::uint64_t r, l, r_next;
  r = counter & 0xFFFFFFFFFFFFFFFFULL;
  l = (counter >> 64) & 0xFFFFFFFFFFFFFFFFULL;
  for (size_t i = 0; i < PHILOX2X64_ROUNDS; ++i) {
    l = mul64_lo(r, 0xD2B74407B1CE6E93ULL);
    r_next = mul64_hi(r, 0xD2B74407B1CE6E93ULL) ^ k0 ^ l;
    r = r_next;
    k1 += 0xBB67AE85;
    k0 += 0x9E3779B9;
  }
  reshi = l;
  reslo = r;
#ifdef RESET_KEYS
  k0 = 0;
  k1 = 0;
#endif
}

const char *generator_name(void) { return NAME; }

void set_seed(std::uint64_t s0, std::uint64_t s1) {
  k0 = static_cast<std::uint32_t>(s0 & 0xFFFFFFFF);
  k1 = static_cast<std::uint32_t>((s0 >> 32) & 0xFFFFFFFF);
  counter = 0;
  count = 0;
}

void set_output_shift(size_t shift) {}

std::uint32_t rand32() {
  switch (count % 4) {
  case 1:
    count++;
    return static_cast<std::uint32_t>(reslo >> 32);
  case 2:
    count++;
    return static_cast<std::uint32_t>(reshi & 0xFFFFFFFFULL);
  case 3:
    count++;
    return static_cast<std::uint32_t>(reshi >> 32);
  default:
    count++;
    NEXT();
    return static_cast<std::uint32_t>(reslo & 0xFFFFFFFFULL);
  }
}

std::uint64_t rand64() {
  if (count++ % 2 == 1) {
    return reshi;
  } else {
    NEXT();
    return reslo;
  }
}
