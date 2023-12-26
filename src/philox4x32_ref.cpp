#include "Random123/philox.h"
#include <cstddef>
#include <cstdint>

#ifdef C_LINKAGE
extern "C" const char *generator_name(void);
extern "C" void set_seed(uint64_t s0, uint64_t s1);
extern "C" uint64_t rand64(void);
extern "C" uint32_t rand32(void);
#endif

typedef r123::Philox4x32 RNG;

// r123array4x32   ctr_type
// r123array2x32   key_type
// r123array2x32   ukey_type

RNG::key_type key = {{0, 0}};
RNG::ctr_type ctr;
RNG *generator;
RNG::ctr_type value;
unsigned count;

const char *generator_name(void) { return "Philox4x32_10_reference"; }

void set_seed(std::uint64_t s0, std::uint64_t s1) {
  generator = new RNG();
  std::uint32_t seed0_lo =
      static_cast<std::uint32_t>((s0 >> 0) & 0xFFFFFFFFULL);
  std::uint32_t seed0_hi =
      static_cast<std::uint32_t>((s0 >> 32) & 0xFFFFFFFFULL);
  std::uint32_t seed1_lo =
      static_cast<std::uint32_t>((s1 >> 0) & 0xFFFFFFFFULL);
  std::uint32_t seed1_hi =
      static_cast<std::uint32_t>((s1 >> 32) & 0xFFFFFFFFULL);
  ctr = {{seed1_hi, seed1_lo, seed0_hi, seed0_lo}};
  count = 0;
}

void set_output_shift(size_t shift) {}

std::uint32_t rand32() {
#ifdef MATCH_OUTPUT
  // Modifications to counter increment and word ordering necessary to match
  // with the custom implementation of this generator.
  switch (count % 4) {
  case 1:
    count++;
    return static_cast<std::uint32_t>(value[1] & 0xFFFFFFFFULL);
  case 2:
    count++;
    return static_cast<std::uint32_t>(value[0] & 0xFFFFFFFFULL);
  case 3:
    count++;
    return static_cast<std::uint32_t>(value[3] & 0xFFFFFFFFULL);
  default: {
    count++;
    value = (*generator)(ctr, key);
    ctr[2]++;
    return static_cast<std::uint32_t>(value[2] & 0xFFFFFFFFULL);
  }
  }
#else
  switch (count % 4) {
  case 1:
    count++;
    return static_cast<std::uint32_t>(value[1] & 0xFFFFFFFFULL);
  case 2:
    count++;
    return static_cast<std::uint32_t>(value[2] & 0xFFFFFFFFULL);
  case 3:
    count++;
    return static_cast<std::uint32_t>(value[3] & 0xFFFFFFFFULL);
  default: {
    count++;
    value = (*generator)(ctr, key);
    ctr.incr();
    return static_cast<std::uint32_t>(value[0] & 0xFFFFFFFFULL);
  }
  }
#endif
}

std::uint64_t rand64() {
  if (count % 2 == 0) {
    count++;
    value = (*generator)(ctr, key);
    ctr.incr();
    return static_cast<std::uint64_t>(value[1]) << 32 |
           static_cast<std::uint64_t>(value[0]);
  } else {
    count++;
    return static_cast<std::uint64_t>(value[3]) << 32 |
           static_cast<std::uint64_t>(value[2]);
  }
}
