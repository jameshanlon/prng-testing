#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <set>
#include <iostream>
#include <vector>
#include <unistd.h>
#include "util.hpp"

extern "C" {
#include "unif01.h"
#include "bbattery.h"
}

const unsigned TEST_ITERATIONS = 1000;

const char *generator_name();
void set_seed(uint64_t, uint64_t);
std::uint32_t rand32();
std::uint64_t rand64();

inline std::uint32_t rand32_rev() {
  return reverse32(rand32());
}

inline std::uint64_t rand64_rev() {
  return reverse64(rand64());
}

inline std::uint32_t rand32_lo() {
  return static_cast<std::uint32_t>(rand64() & 0xFFFFFFFFULL);
}

inline std::uint32_t rand32_hi() {
  return static_cast<std::uint32_t>((rand64() >> 32) & 0xFFFFFFFFULL);
}

inline std::uint32_t rand32_lo_rev() {
  return reverse32(rand32_lo());
}

inline std::uint32_t rand32_hi_rev() {
  return reverse32(rand32_hi());
}

void error32(const char* name, size_t iteration, std::uint32_t outputBase, std::uint32_t outputCompare) {
  std::cerr << name << " does not match, output at " << std::dec << iteration << ": " << std::hex << outputBase
            << " != " << outputCompare << "\n";
}

/// Test a generator with 32-bit output.
void test32_32(std::uint64_t s0, std::uint64_t s1) {
  std::vector<std::uint32_t> output(TEST_ITERATIONS);
  // Generate some output.
  set_seed(s0, s1);
  for (size_t i = 0; i < TEST_ITERATIONS; i++) {
    output[i] = rand32();
  }
  // Check each value in the output is unique.
  std::set<std::uint32_t> outputSet(output.begin(), output.end());
  if (outputSet.size() != output.size()) {
    std::cerr << "32 bit output values are not unique\n";
  }
  // Check rand32_rev
  set_seed(s0, s1);
  for (size_t i = 0; i < TEST_ITERATIONS; i++) {
    std::uint32_t rand32 = rand32_rev();
    if (reverse32(output[i]) != rand32) {
      std::cerr << "rand32_rev does not match, output at " << std::dec << i << ": " << std::hex << reverse32(output[i])
                << " != " << rand32 << "\n";
    }
  }
}

/// Test a generate with 64-bit output where 32 bits is produced from the high
/// and low portions.
void test64_32(std::uint64_t s0, std::uint64_t s1) {
  std::vector<std::uint32_t> output(TEST_ITERATIONS);
  // Generate some output.
  set_seed(s0, s1);
  for (size_t i = 0; i < TEST_ITERATIONS; i++) {
    output[i] = rand32();
  }
  // Check each value in the output is unique.
  std::set<std::uint32_t> outputSet(output.begin(), output.end());
  if (outputSet.size() != output.size()) {
    std::cerr << "32-bit output values are not unique\n";
  }
  // Check rand32_rev
  set_seed(s0, s1);
  for (size_t i = 0; i < TEST_ITERATIONS; i++) {
    std::uint32_t rand32 = rand32_rev();
    if (reverse32(output[i]) != rand32) {
      error32("rand32_rev", i, reverse32(output[i]), rand32);
    }
  }
  // Check rand32_lo
  set_seed(s0, s1);
  for (size_t i = 0; i < TEST_ITERATIONS; i += 2) {
    std::uint32_t rand32 = rand32_lo();
    if (output[i] != rand32) {
      error32("rand32_lo", i, output[i], rand32);
    }
  }
  // Check rand32_hi
  set_seed(s0, s1);
  for (size_t i = 1; i < TEST_ITERATIONS; i += 2) {
    std::uint32_t rand32 = rand32_hi();
    if (output[i] != rand32) {
      error32("rand32_hi", i, output[i], rand32);
    }
  }
  // Check rand32_lo_rev
  set_seed(s0, s1);
  for (size_t i = 0; i < TEST_ITERATIONS; i += 2) {
    std::uint32_t rand32 = rand32_lo_rev();
    if (reverse32(output[i]) != rand32) {
      error32("rand32_lo_rev", i, reverse32(output[i]), rand32);
    }
  }
  // Check rand32_hi_rev
  set_seed(s0, s1);
  for (size_t i = 1; i < TEST_ITERATIONS; i += 2) {
    std::uint32_t rand32 = rand32_hi_rev();
    if (reverse32(output[i]) != rand32) {
      error32("rand32_hi_rev", i, reverse32(output[i]), rand32);
    }
  }
}

/// Test the 64-bit output of a 64-bit generator.
void test64_64(std::uint64_t s0, std::uint64_t s1) {
  std::vector<std::uint64_t> output(TEST_ITERATIONS);
  // Generate some output.
  set_seed(s0, s1);
  for (size_t i = 0; i < TEST_ITERATIONS; i++) {
    output[i] = rand64();
  }
  // Check each value in the output is unique.
  std::set<std::uint64_t> outputSet(output.begin(), output.end());
  if (outputSet.size() != output.size()) {
    std::cerr << "64 bit output values are not unique\n";
  }
  // Check rand64_rev
  set_seed(s0, s1);
  for (size_t i = 0; i < TEST_ITERATIONS; i++) {
    std::uint64_t rand64 = rand64_rev();
    if (reverse64(output[i]) != rand64) {
      std::cerr << "rand64_rev does not match, output at " << std::dec << i << ": " << std::hex << reverse64(output[i])
                << " != " << rand64 << "\n";
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc != 5) {
    std::cerr << argv[0] << " <command> <output> <seed64lo> <seed64hi>\n";
    std::cerr << "  command  (test, stdout, smallcrush, crush, bigcrush)\n";
    std::cerr << "  output   (std32, rev32, std64, rev64, std32lo, rev32lo, std32hi, rev32hi)\n";
    return 1;
  }
  std::uint64_t s0, s1;
  bool          run_tests = std::string(argv[1]) == "test";
  bool          std_out   = std::string(argv[1]) == "stdout";
  bool          std32     = std::string(argv[2]) == "std32";
  bool          rev32     = std::string(argv[2]) == "rev32";
  bool          std64     = std::string(argv[2]) == "std64";
  bool          rev64     = std::string(argv[2]) == "rev64";
  bool          std32lo   = std::string(argv[2]) == "std32lo";
  bool          rev32lo   = std::string(argv[2]) == "rev32lo";
  bool          std32hi   = std::string(argv[2]) == "std32hi";
  bool          rev32hi   = std::string(argv[2]) == "rev32hi";
  s0                      = std::strtoull(argv[3], NULL, 0);
  s1                      = std::strtoull(argv[4], NULL, 0);
  // clang-format off
  auto gen32 = std32   ? rand32 :
               rev32   ? rand32_rev :
               std32lo ? rand32_lo :
               rev32lo ? rand32_lo_rev :
               std32hi ? rand32_hi :
               rev32hi ? rand32_hi_rev :
                         nullptr;
  auto gen64 = std64 ? rand64 :
               rev64 ? rand64_rev :
                       nullptr;
  // clang-format on
  if (run_tests) {
    std::cout << "Testing " << generator_name() << "\n";
#ifdef TEST_32BIT_OUTPUT
    test32_32(s0, s1);
#else // 64BIT_OUTPUT
    test64_32(s0, s1);
    test64_64(s0, s1);
#endif
    return 0;
  }
  set_seed(s0, s1);
  if (std_out) {
    freopen(nullptr, "wb", stdout);
    while (1) {
      if (std32 || rev32 || std32lo || rev32lo) {
        // 32-bit outputs.
        constexpr size_t BUFFER_SIZE = (1024 * 1024) / sizeof(std::uint32_t);
        static uint32_t  buffer[BUFFER_SIZE];
        for (size_t j = 0; j < BUFFER_SIZE; ++j) {
          std::uint32_t rand = gen32();
          std::memcpy(&buffer[j], &rand, sizeof(std::uint32_t));
        }
        fwrite(static_cast<void*>(buffer), sizeof(buffer[0]), BUFFER_SIZE, stdout);
      } else {
        // 64-bit outputs.
        constexpr size_t BUFFER_SIZE = (1024 * 1024) / sizeof(std::uint64_t);
        static uint64_t  buffer[BUFFER_SIZE];
        for (size_t j = 0; j < BUFFER_SIZE; ++j) {
          std::uint64_t rand = gen64();
          std::memcpy(&buffer[j], &rand, sizeof(uint64_t));
        }
        fwrite(static_cast<void*>(buffer), sizeof(buffer[0]), BUFFER_SIZE, stdout);
      }
    }
  } else {
    if (std64 || rev64) {
      std::cerr << "64 bit output not supported for TestU01.\n";
      return 1;
    }
    unif01_Gen* gen = unif01_CreateExternGenBits(const_cast<char*>(generator_name()), gen32);
    if (std::string(argv[1]) == "smallcrush") {
      bbattery_SmallCrush(gen);
    } else if (std::string(argv[1]) == "crush") {
      bbattery_Crush(gen);
    } else if (std::string(argv[1]) == "bigcrush") {
      bbattery_BigCrush(gen);
    }
  }
  return 0;
}
