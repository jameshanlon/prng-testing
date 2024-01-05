#include "util.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <set>
#include <unistd.h>
#include <vector>

extern "C" {
#include "bbattery.h"
#include "scomp.h"
#include "smarsa.h"
#include "smultin.h"
#include "unif01.h"
}

const unsigned TEST_ITERATIONS = 1000;

const char *generator_name();
void set_seed(uint64_t, uint64_t);
void set_output_shift(size_t);
uint32_t rand32();
uint64_t rand64();

inline uint32_t rand32_rev() { return reverse32(rand32()); }

inline uint64_t rand64_rev() { return reverse64(rand64()); }

inline uint32_t rand32_lo() {
  return static_cast<uint32_t>(rand64() & 0xFFFFFFFFULL);
}

inline uint32_t rand32_hi() {
  return static_cast<uint32_t>((rand64() >> 32) & 0xFFFFFFFFULL);
}

inline uint32_t rand32_lo_rev() { return reverse32(rand32_lo()); }

inline uint32_t rand32_hi_rev() { return reverse32(rand32_hi()); }

void error32(const char *name, size_t iteration, uint32_t outputBase,
             uint32_t outputCompare) {
  std::cerr << name << " does not match, output at " << std::dec << iteration
            << ": " << std::hex << outputBase << " != " << outputCompare
            << "\n";
}

/// Test a generator with 32-bit output.
void test32_32(uint64_t s0, uint64_t s1) {
  std::vector<uint32_t> output(TEST_ITERATIONS);
  // Generate some output.
  set_seed(s0, s1);
  for (size_t i = 0; i < TEST_ITERATIONS; i++) {
    output[i] = rand32();
  }
  // Check each value in the output is unique.
  std::set<uint32_t> outputSet(output.begin(), output.end());
  if (outputSet.size() != output.size()) {
    std::cerr << "32 bit output values are not unique\n";
  }
  // Check rand32_rev
  set_seed(s0, s1);
  for (size_t i = 0; i < TEST_ITERATIONS; i++) {
    uint32_t rand32 = rand32_rev();
    if (reverse32(output[i]) != rand32) {
      std::cerr << "rand32_rev does not match, output at " << std::dec << i
                << ": " << std::hex << reverse32(output[i]) << " != " << rand32
                << "\n";
    }
  }
}

/// Test a generate with 64-bit output where 32 bits is produced from the high
/// and low portions.
void test64_32(uint64_t s0, uint64_t s1) {
  std::vector<uint32_t> output(TEST_ITERATIONS);
  // Generate some output.
  set_seed(s0, s1);
  for (size_t i = 0; i < TEST_ITERATIONS; i++) {
    output[i] = rand32();
  }
  // Check each value in the output is unique.
  std::set<uint32_t> outputSet(output.begin(), output.end());
  if (outputSet.size() != output.size()) {
    std::cerr << "32-bit output values are not unique\n";
  }
  // Check rand32_rev
  set_seed(s0, s1);
  for (size_t i = 0; i < TEST_ITERATIONS; i++) {
    uint32_t rand32 = rand32_rev();
    if (reverse32(output[i]) != rand32) {
      error32("rand32_rev", i, reverse32(output[i]), rand32);
    }
  }
  // Check rand32_lo
  set_seed(s0, s1);
  for (size_t i = 0; i < TEST_ITERATIONS; i += 2) {
    uint32_t rand32 = rand32_lo();
    if (output[i] != rand32) {
      error32("rand32_lo", i, output[i], rand32);
    }
  }
  // Check rand32_hi
  set_seed(s0, s1);
  for (size_t i = 1; i < TEST_ITERATIONS; i += 2) {
    uint32_t rand32 = rand32_hi();
    if (output[i] != rand32) {
      error32("rand32_hi", i, output[i], rand32);
    }
  }
  // Check rand32_lo_rev
  set_seed(s0, s1);
  for (size_t i = 0; i < TEST_ITERATIONS; i += 2) {
    uint32_t rand32 = rand32_lo_rev();
    if (reverse32(output[i]) != rand32) {
      error32("rand32_lo_rev", i, reverse32(output[i]), rand32);
    }
  }
  // Check rand32_hi_rev
  set_seed(s0, s1);
  for (size_t i = 1; i < TEST_ITERATIONS; i += 2) {
    uint32_t rand32 = rand32_hi_rev();
    if (reverse32(output[i]) != rand32) {
      error32("rand32_hi_rev", i, reverse32(output[i]), rand32);
    }
  }
}

/// Test the 64-bit output of a 64-bit generator.
void test64_64(uint64_t s0, uint64_t s1) {
  std::vector<uint64_t> output(TEST_ITERATIONS);
  // Generate some output.
  set_seed(s0, s1);
  for (size_t i = 0; i < TEST_ITERATIONS; i++) {
    output[i] = rand64();
  }
  // Check each value in the output is unique.
  std::set<uint64_t> outputSet(output.begin(), output.end());
  if (outputSet.size() != output.size()) {
    std::cerr << "64 bit output values are not unique\n";
  }
  // Check rand64_rev
  set_seed(s0, s1);
  for (size_t i = 0; i < TEST_ITERATIONS; i++) {
    uint64_t rand64 = rand64_rev();
    if (reverse64(output[i]) != rand64) {
      std::cerr << "rand64_rev does not match, output at " << std::dec << i
                << ": " << std::hex << reverse64(output[i]) << " != " << rand64
                << "\n";
    }
  }
}

void help(const char *argv[]) {
  std::cerr << argv[0] << " command output seed64lo seed64hi\n";
  std::cerr << "\n";
  std::cerr << "Positional parameters:\n";
  std::cerr << "  command  [stdout, smallcrush, crush, bigcrush, alphabit, "
               "matrixrank, linearcomp, test, time, analyse]\n";
  std::cerr << "  output   [std32, rev32, std64, rev64, std32lo, rev32lo, "
               "std32hi, rev32hi]\n";
  std::cerr << "  seed64hi 64 lowest bits of the 128-bit seed\n";
  std::cerr << "  seed64lo 64 highest bits of the 128-bit seed\n";
  std::cerr << "\n";
  std::cerr << "Optional parameters:\n";
  std::cerr << "  --output-shift N  Number of bits to shift the generator "
               "output by\n";
}

int main(int argc, const char *argv[]) {

  // Parse positional arguments.
  if (argc < 5) {
    help(argv);
    std::exit(1);
  }
  // Command
  bool run_tests = std::string(argv[1]) == "test";
  bool time_output = std::string(argv[1]) == "time";
  bool analyse_output = std::string(argv[1]) == "analyse";
  bool std_out = std::string(argv[1]) == "stdout";
  // Output
  bool std32 = std::string(argv[2]) == "std32";
  bool rev32 = std::string(argv[2]) == "rev32";
  bool std64 = std::string(argv[2]) == "std64";
  bool rev64 = std::string(argv[2]) == "rev64";
  bool std32lo = std::string(argv[2]) == "std32lo";
  bool rev32lo = std::string(argv[2]) == "rev32lo";
  bool std32hi = std::string(argv[2]) == "std32hi";
  bool rev32hi = std::string(argv[2]) == "rev32hi";
  // Seeds
  uint64_t s0 = std::strtoull(argv[3], NULL, 0);
  uint64_t s1 = std::strtoull(argv[4], NULL, 0);

  // Parse optional arguments.
  size_t outputShift = 0;
  for (int i = 5; i < argc; ++i) {
    if (std::strcmp(argv[i], "-h") == 0 ||
        std::strcmp(argv[i], "--help") == 0) {
      help(argv);
      std::exit(1);
    } else if (std::strcmp(argv[i], "--output-shift") == 0) {
      outputShift = std::strtoul(argv[++i], NULL, 0);
      if (outputShift > 63 || outputShift < 0) {
        std::cerr << "Invalid output shift\n";
        std::exit(1);
      }
    }
  }

  // Setup generator function.
  auto gen32 = std32     ? rand32
               : rev32   ? rand32_rev
               : std32lo ? rand32_lo
               : rev32lo ? rand32_lo_rev
               : std32hi ? rand32_hi
               : rev32hi ? rand32_hi_rev
                         : nullptr;
  auto gen64 = std64 ? rand64 : rev64 ? rand64_rev : nullptr;

  // Test the generator.
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

  set_output_shift(outputShift);
  set_seed(s0, s1);

  // Check for duplicate values in a fixed portion of output.
  if (analyse_output) {
    constexpr size_t BUFFER_LENGTH = 1024 * 1024 * 128;
    constexpr size_t BUFFER_SIZE_BYTES =
        BUFFER_LENGTH * sizeof(uint64_t); // 1GB
    uint64_t *buffer = (uint64_t *)malloc(BUFFER_SIZE_BYTES);
    std::cout << "Checking " << BUFFER_SIZE_BYTES / (1024 * 1024)
              << " MB of output\n";
    std::cout << "Generating data...\n";
    for (size_t j = 0; j < BUFFER_LENGTH; ++j) {
      buffer[j] = gen64();
    }
    std::cout << "Sorting buffer...\n";
    std::sort(buffer, buffer + BUFFER_LENGTH);
    std::cout << "Checking duplicates...\n";
    for (size_t i = 0; i < BUFFER_LENGTH - 1; i++) {
      if (buffer[i] == buffer[i + 1]) {
        std::cout << "Duplicate " << std::to_string(buffer[i]) << "\n";
      }
    }
    free(buffer);
    return 0;
  }

  // Time the generator.
  if (time_output) {
    if (std32 || rev32 || std32lo || rev32lo) {
      std::cerr << "Only 64 bit output is supported for timing\n;";
      return 1;
    }
    size_t iterations = 1000000000; // 1bn iterations.
    uint64_t result = 0;            // To prevent the calls being optimised out.
    double totalBytes = 8 * (double)iterations;
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
      result += gen64();
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    double bytesPerMs = totalBytes / (double)duration.count();
    double nsPer64bits =
        (double)(duration.count() * 1000ULL) / (double)iterations;
    std::cout << "Result " << result << std::endl;
    std::cout << "Bytes output per microsecond:   " << bytesPerMs << std::endl;
    std::cout << "Nanoseconds per 64 bits output: " << nsPer64bits << std::endl;
    return 0;
  }

  // Output to stdout.
  if (std_out) {
    freopen(nullptr, "wb", stdout);
    while (1) {
      if (std32 || rev32 || std32lo || rev32lo) {
        // 32-bit outputs.
        constexpr size_t BUFFER_SIZE = (1024 * 1024) / sizeof(uint32_t);
        static uint32_t buffer[BUFFER_SIZE];
        for (size_t j = 0; j < BUFFER_SIZE; ++j) {
          uint32_t rand = gen32();
          std::memcpy(&buffer[j], &rand, sizeof(uint32_t));
        }
        fwrite(static_cast<void *>(buffer), sizeof(buffer[0]), BUFFER_SIZE,
               stdout);
      } else {
        // 64-bit outputs.
        constexpr size_t BUFFER_SIZE = (1024 * 1024) / sizeof(uint64_t);
        static uint64_t buffer[BUFFER_SIZE];
        for (size_t j = 0; j < BUFFER_SIZE; ++j) {
          uint64_t rand = gen64();
          std::memcpy(&buffer[j], &rand, sizeof(uint64_t));
        }
        fwrite(static_cast<void *>(buffer), sizeof(buffer[0]), BUFFER_SIZE,
               stdout);
      }
    }
    return 0;
  }

  // Run the *crush batteries.
  if (std64 || rev64) {
    std::cerr << "64 bit output not supported for TestU01.\n";
    return 1;
  }
  unif01_Gen *gen =
      unif01_CreateExternGenBits(const_cast<char *>(generator_name()), gen32);
  if (std::string(argv[1]) == "smallcrush") {
    bbattery_SmallCrush(gen);
  } else if (std::string(argv[1]) == "crush") {
    bbattery_Crush(gen);
  } else if (std::string(argv[1]) == "bigcrush") {
    bbattery_BigCrush(gen);
  } else if (std::string(argv[1]) == "alphabit") {
    double numBits = (1024ULL * 1024ULL * 1024ULL * 8ULL * 1024ULL); // 1 TB
    bbattery_Alphabit(gen, numBits, 0, 32);
  } else if (std::string(argv[1]) == "matrixrank") {
    // LSB taken from each uniform.
    // 10000 x 10000 matrix.
    smarsa_MatrixRank(gen, nullptr, 1, 100, 32 - 1, 1, 10000, 10000);
  } else if (std::string(argv[1]) == "linearcomp") {
    // LSB taken from each uniform.
    // Use recurrances of at most 800000 previous outputs.
    scomp_LinearComp(gen, nullptr, 1, 800000, 32 - 1, 1);
  }
  return 0;
}
