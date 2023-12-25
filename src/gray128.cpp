#include <iostream>
#include <cstdint>
#include <vector>

__uint128_t gray(__uint128_t value) {
  // Gray code the counter by XORing adjacent bits Gn = Cn+1 XOR Cn (for n=0 to 126) G127 = C127.
  value ^= ((value << 127) | (value >> 1));
  return value;
}

int main(int argc, char *argv[]) {
  __uint128_t counter = 0;
  std::vector<__uint128_t> grayCounter;
  for (size_t i=0; i<100; i++) {
    counter++;
    grayCounter.push_back(gray(counter));
  }
  for (size_t i=0; i<99; i++) {
    std::cout << __builtin_popcount(grayCounter[i] ^ grayCounter[i+1]) << "\n";
  }
  return 0;
}
