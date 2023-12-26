#ifndef UTIL_HPP
#define UTIL_HPP

#include <cstdint>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define PASTER(x, y) x##y
#define CONCAT(x, y) PASTER(x, y)

static inline std::uint64_t rotl(std::uint64_t x, int k) {
  return (x << k) | (x >> (64 - k));
}

static inline std::uint64_t rotr(std::uint64_t x, int k) {
  return (x >> k) | (x << (64 - k));
}

static inline std::uint64_t aox(std::uint64_t s0, std::uint64_t s1) {
  std::uint64_t sx = s1 ^ s0;
  std::uint64_t carry = s1 & s0;
  return sx ^ (rotl(carry, 1) | rotl(carry, 2));
}

static inline std::uint32_t reverse32(std::uint32_t x) {
  x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
  x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
  x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
  x = (((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8));
  return ((x >> 16) | (x << 16));
}

static inline std::uint64_t reverse64(std::uint64_t x) {
  x = ((x >> 1) & 0x5555555555555555ULL) | ((x & 0x5555555555555555ULL) << 1);
  x = ((x >> 2) & 0x3333333333333333ULL) | ((x & 0x3333333333333333ULL) << 2);
  x = ((x >> 4) & 0x0F0F0F0F0F0F0F0FULL) | ((x & 0x0F0F0F0F0F0F0F0FULL) << 4);
  x = ((x >> 8) & 0x00FF00FF00FF00FFULL) | ((x & 0x00FF00FF00FF00FFULL) << 8);
  x = ((x >> 16) & 0x0000FFFF0000FFFFULL) | ((x & 0x0000FFFF0000FFFFULL) << 16);
  return (x >> 32) | (x << 32);
}

#endif // UTIL_HPP
