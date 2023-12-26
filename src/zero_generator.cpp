#include <cstddef>
#include <cstdint>

const char *generator_name(void) { return "Zero generator"; }

void set_seed(uint64_t s0, uint64_t s1) {}

void set_output_shift(size_t shift) {}

std::uint32_t rand32() { return 0; }

std::uint64_t rand64() { return 0ULL; }
