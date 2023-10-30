#include <cstdio>
#include <cstdint>

#include <random>

#include <boost/random/mersenne_twister.hpp>

// std::mt19937* generator;
boost::mt19937* generator;

const char* generator_name(void) {
  return "mt32";
}

///// Boost implementation supports setting the seed directly.
// void set_seed(std::uint64_t s0, std::uint64_t s1) {
//  std::vector<unsigned> state(624, 0);
//  state[0]                           = static_cast<unsigned>(s0 & 0xFFFFFFFFULL);
//  state[1]                           = static_cast<unsigned>(s0 >> 32);
//  generator                          = new boost::mt19937();
//  std::vector<unsigned>::iterator it = state.begin();
//  generator->seed(it, state.end());
//}

/// Boost mt19937.
void set_seed(std::uint64_t s0, std::uint64_t s1) {
  generator = new boost::mt19937(static_cast<unsigned int>(s1 >> 32));
}

//// std mt19937.
// void set_seed(std::uint64_t s0, std::uint64_t s1) {
//  generator = new std::mt19937(static_cast<unsigned int>(s1>>32));
//}

std::uint32_t rand32() {
  return (*generator)();
}

std::uint64_t rand64() {
  return static_cast<uint64_t>(rand32()) | (static_cast<uint64_t>(rand32()) << 32);
}