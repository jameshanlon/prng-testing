#include <cstddef>
#include <cstdint>
#include <unistd.h>

#include "pcg_random.hpp"

std::uint64_t seed;
std::uint64_t value;
int count;
pcg32_fast *generator;

const char *generator_name(void) {
  return "PCG32_fast";
}

void set_seed(std::uint64_t s0, std::uint64_t s1) {
  // Ignore s1
  generator = new pcg32_fast(s0);
  count = 0;
}

void set_output_shift(size_t shift) {}

std::uint32_t rand32() {
  return (*generator)();
}

std::uint64_t rand64() {
  return static_cast<std::uint64_t>((*generator)()) << 32 |
         static_cast<std::uint64_t>((*generator)());
}
