/* Sample file for hwd.c (xoroshiro128+).

static inline uint64_t rotl(const uint64_t x, int k) {
        return (x << k) | (x >> (64 - k));
}

static uint64_t s[2] = { 1, -1ULL };

static uint64_t inline next() {
        const uint64_t s0 = s[0];
        uint64_t s1 = s[1];

        const uint64_t result_plus = s0 + s1;

        s1 ^= s0;
        s[0] = rotl(s0, 24) ^ s1 ^ (s1 << 16);
        s[1] = rotl(s1, 37);

        return result_plus;
}
*/

#include <stdint.h>

extern uint64_t rand64(void);
extern void     set_seed(uint64_t, uint64_t);

static uint64_t inline next() {
  return rand64();
}
