#include <cstdint>

const char *generator_name(void) {
  return "Zero generator";
}

void set_seed(std::uint64_t s0, std::uint64_t s1) {
}

std::uint32_t rand32() {
  return 0;
}

std::uint64_t rand64() {
  return 0ULL;
}
