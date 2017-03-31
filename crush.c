#include "unif01.h"
#include "bbattery.h"
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

uint64_t s0, s1, res;
unsigned select;

static inline uint64_t rotl(const uint64_t x, int k) {
  return (x << k) | (x >> (64 - k));
}

#ifdef REVERSE
static inline uint64_t reverse(uint64_t x) {
  x = ((x >> 1)  & 0x5555555555555555ULL) | ((x & 0x5555555555555555ULL) << 1);
  x = ((x >> 2)  & 0x3333333333333333ULL) | ((x & 0x3333333333333333ULL) << 2);
  x = ((x >> 4)  & 0x0F0F0F0F0F0F0F0FULL) | ((x & 0x0F0F0F0F0F0F0F0FULL) << 4);
  x = ((x >> 8)  & 0x00FF00FF00FF00FFULL) | ((x & 0x00FF00FF00FF00FFULL) << 8);
  x = ((x >> 16) & 0x0000FFFF0000FFFFULL) | ((x & 0x0000FFFF0000FFFFULL) << 16);
  return (x >> 32) | (x << 32);
}
#endif

uint64_t next(void) {
  const uint64_t res = s0 + s1;
  s1 ^= s0;
  s0 = rotl(s0, 55) ^ s1 ^ (s1 << 14);
  s1 = rotl(s1, 36);
#ifdef REVERSE
  return reverse(res);
#else
  return res;
#endif
}

unsigned int rand32() {
  if (select ^= ~0U) {
    res = next();
    return res & 0xFFFFFFFFULL;
  }
  return (res >> 32) & 0xFFFFFFFFULL;
}

int main(int argc, char *argv[])  {
  select = 0;
  if (argc != 3) {
    fprintf(stderr, "%s s0 s1\n", argv[0]);
    exit(1);
  }
  s0 = strtoull(argv[1], NULL, 0);
  s1 = strtoull(argv[2], NULL, 0);
  unif01_Gen *gen = unif01_CreateExternGenBits("xoroshiro128+", rand32);
  bbattery_SmallCrush(gen);
  //bbattery_Crush(gen);
  //bbattery_BigCrush(gen);
  return 0;
}

