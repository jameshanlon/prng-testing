#include <bitset>
#include <cstdint>
#include <iostream>
#include <vector>

const char *generator_name();
void set_seed(uint64_t, uint64_t);
std::uint32_t rand32();
std::uint64_t rand64();

/// The number of one-hot seeds to use.
#ifdef STATE32_OUTPUT32
constexpr size_t NUM_SEEDS = 32;
#endif
#ifdef STATE128_OUTPUT64
constexpr size_t NUM_SEEDS = 128;
#endif

int main(int argc, char *argv[]) {

  // Handle arguments.
  if (argc != 4) {
    std::cerr << "USAGE: " << argv[0]
              << " <steps> <sample-inerval> <window-size>\n";
    std::cerr << "  Steps:           The number of generator iterations to run "
                 "for.\n";
    std::cerr << "  Window size:     The number of previous outputs over which "
                 "to compute an average.\n";
    std::cerr << "  Sample interval: The interval over which to report results "
                 "(to summarise large nubmers of steps).\n";
    return 1;
  }
  size_t num_steps = std::strtoul(argv[1], nullptr, 0);
  size_t sample_interval = std::strtoul(argv[2], nullptr, 0);
  size_t window_size = std::strtoul(argv[3], nullptr, 0);

  // Gather results
  std::vector<double> results[NUM_SEEDS];
  for (size_t i = 0; i < NUM_SEEDS; i++) {
#ifdef STATE32_OUTPUT32
    // 32 bit state, 32 bit output
    uint64_t seed = 1 << i;
    set_seed(seed, 0);
    for (size_t step = 0; step < num_steps; step++) {
      uint32_t res = rand32();
      size_t setBits = __builtin_popcount(res);
      double setBitsFrac = static_cast<double>(setBits) / 32.0;
      results[i].push_back(setBitsFrac);
    }
#endif
#ifdef STATE128_OUTPUT64
    // 128 bit state, 64 bit output
    uint64_t seed0 = i < 64 ? 1 << i : 0ULL;
    uint64_t seed1 = i >= 64 ? 1 << (i - 64) : 0ULL;
    set_seed(seed0, seed1);
    for (size_t step = 0; step < num_steps; step++) {
      uint64_t res = rand64();
      size_t setBits = __builtin_popcountll(res);
      double setBitsFrac = static_cast<double>(setBits) / 64.0;
      // std::bitset<64> binary(res);
      // std::cout << " seed " << i << ", step " << step << " set bits " <<
      // setBits << " (" << (setBitsFrac*100.0) << "%) value " << binary <<
      // "\n";
      results[i].push_back(setBitsFrac);
    }
#endif
  }

  // Compute averages.
  std::vector<double> averages;
  for (size_t step = window_size - 1; step < num_steps; step++) {
    double seedAverage = 0.0;
    for (size_t i = 0; i < NUM_SEEDS; i++) {
      double total = 0.0;
      for (size_t j = 0; j < window_size; j++) {
        total += results[i][step - j];
      }
      seedAverage += total / static_cast<double>(window_size);
    }
    averages.push_back(seedAverage / NUM_SEEDS);
  }

  // Report results.
  size_t i = 1;
  for (auto value : averages) {
    if (i % sample_interval == 0) {
      std::cout << i << " " << value << "\n";
    }
    i++;
  }
  return 0;
}
