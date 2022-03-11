#include <cstddef>
#include <cstdint>
#include <unistd.h>

#include "pcg_random.hpp"

std::uint64_t seed;
std::uint64_t value;
int count;
pcg64_fast *generator;

const char *generator_name(void) {
  return "PCG64_fast";
}

void set_seed(std::uint64_t s0, std::uint64_t s1) {
  // Ignore s1
  generator = new pcg64_fast(s0);
  count = 0;
}

std::uint32_t rand32() {
  if (count++ % 2) {
    return static_cast<std::uint32_t>(value >> 32);
  } else {
    value = (*generator)();
    return static_cast<std::uint32_t>(value & 0xFFFFFFFFULL);
  }
}

std::uint64_t rand64() {
  return (*generator)();
}
