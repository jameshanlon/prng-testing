#include <cstddef>
#include <cstdint>
#include <unistd.h>

#include "pcg_random.hpp"

#ifdef C_LINKAGE
extern "C" const char *generator_name(void);
extern "C" void set_seed(uint64_t s0, uint64_t s1);
extern "C" uint64_t rand64(void);
extern "C" uint32_t rand32(void);
#endif

std::uint64_t seed;
std::uint64_t value;
int count;
pcg64 *generator;

const char *generator_name(void) { return "PCG64"; }

void set_seed(std::uint64_t s0, std::uint64_t s1) {
  generator = new pcg64(PCG_128BIT_CONSTANT(s1, s0));
  count = 0;
}

void set_output_shift(size_t shift) {}

std::uint32_t rand32() {
  if (count++ % 2) {
    return static_cast<std::uint32_t>(value >> 32);
  } else {
    value = (*generator)();
    return static_cast<std::uint32_t>(value & 0xFFFFFFFFULL);
  }
}

std::uint64_t rand64() { return (*generator)(); }
