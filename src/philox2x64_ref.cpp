#include <cstddef>
#include <cstdint>
#include "Random123/philox.h"

typedef r123::Philox4x64 RNG;

// r123array2x64   ctr_type
// r123array1x64   key_type
// r123array1x64   ukey_type

RNG::key_type key;
RNG::ctr_type ctr;
RNG *generator;
RNG::ctr_type value;
unsigned count;

const char *generator_name(void) {
  return "Philox2x64_10";
}

void set_seed(std::uint64_t s0, std::uint64_t s1) {
  generator = new RNG();
  // Set k0 to be the seed.
  key = {{s0}};
  ctr   = {{0, 0}};
  count = 0;
}

std::uint32_t rand32() {
  switch (count % 4) {
    case 1: count++; return static_cast<std::uint32_t>(value[0] >> 32);
    case 2: count++; return static_cast<std::uint32_t>(value[1] & 0xFFFFFFFFULL);
    case 3: count++; return static_cast<std::uint32_t>(value[1] >> 32);
    default:
      count++;
      ctr.incr();
      value = (*generator)(ctr, key);
      return static_cast<std::uint32_t>(value[0] & 0xFFFFFFFFULL);
  }
}

std::uint64_t rand64() {
  if (count++ % 2 == 1) {
    return value[1];
  } else {
    ctr.incr();
    value = (*generator)(ctr, key);
    return value[0];
  }
}
