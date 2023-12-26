#include <boost/test/unit_test.hpp>

#define SHIFT_A 55
#define SHIFT_B 14
#define SHIFT_C 36
#define SUFFIX aox
#define SEED_EQUIA

#include "../src/xoroshiro128_interleave.cpp"

uint64_t s0_table_ref[NUM_GENERATORS];
uint64_t s1_table_ref[NUM_GENERATORS];
uint64_t s0_ref;
uint64_t s1_ref;

static uint64_t next_xoroshiro128aox_ref(void) {
  // Result returned
  uint64_t res = aox(s0_ref, s1_ref);
  // State update
  uint64_t sx = s0_ref ^ s1_ref;
  s0_ref = rotl(s0_ref, SHIFT_A) ^ sx ^ (sx << SHIFT_B);
  s1_ref = rotl(sx, SHIFT_C);
  return res;
}

BOOST_AUTO_TEST_SUITE(xoroshiro_interleave);

BOOST_AUTO_TEST_CASE(first_match) {
  set_seed(0, 0);
  std::memcpy(&s0_table_ref, &s0, sizeof(uint64_t) * NUM_GENERATORS);
  std::memcpy(&s1_table_ref, &s1, sizeof(uint64_t) * NUM_GENERATORS);
  // Test that the first outputs from the generator match the reference.
  for (size_t i = 0; i < NUM_GENERATORS; i++) {
    uint64_t result = rand64();
    s0_ref = s0_table_ref[i];
    s1_ref = s1_table_ref[i];
    uint64_t reference = next_xoroshiro128aox_ref();
    BOOST_TEST(result == reference);
  }
}

BOOST_AUTO_TEST_SUITE_END();
