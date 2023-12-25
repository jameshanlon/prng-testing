#include <iostream>
#include <vector>

// Chi squared critical values for degrees of freedom = 2^n - 1 and a
// significance level of 0.05
// for bits in range(2,21): print(scipy.stats.chi2.ppf(1-.95, df=(2**bits)-1))

std::vector<double> chi2CriticalVals = {
 0.0,
 0.0,
 6.251388631170325,
 12.017036623780532,
 22.307129581578693,
 41.42173582978522,
 77.74538483569489,
 147.80481300658042,
 284.3359078234513,
 552.3739332206293,
 1081.3794441735477,
 2129.4155912992337,
 4211.398461928273,
 8355.45087491455,
 16615.402567626777,
 33095.49746363793,
 65999.39382911302,
 131727.57865795962,
 263071.3678797117,
 525599.7350561608,
 1050431.3117452222
};

constexpr size_t MIN_NUM_BITS = 2;
constexpr size_t MAX_NUM_BITS = 20;

uint64_t rotl(uint64_t x, int k, int width) {
  uint64_t maskLo = (1<<(width-k)) - 1;
  uint64_t mask = (1<<(width)) - 1;
  return ((x << k) | ((x >> (width - k)) & maskLo)) & mask;
}

uint64_t aox(uint64_t s0, uint64_t s1, uint64_t width) {
  uint64_t sx = s1 ^ s0;
  uint64_t sa = s1 & s0;
  return sx ^ (rotl(sa, 1, width) | rotl(sa, 2, width));
}

uint64_t plus(uint64_t s0, uint64_t s1) {
  return s0 + s1;
}

uint64_t g_pattern;
uint64_t g_numPatterns;
uint64_t g_numBits;

std::uint32_t rand32() {
  uint64_t mask = g_numPatterns - 1;
  uint64_t s0 = g_pattern & mask;
  uint64_t s1 = (g_pattern >> g_numBits) & mask;
  uint64_t outputPattern = aox(s0, s1, g_numBits) & mask;
  return outputPattern;
}

int main(int argc, const char *argv[]) {

  for (size_t numBits=MIN_NUM_BITS; numBits<=MAX_NUM_BITS; numBits++) {
    std::cout << "Num state bits " << (numBits*2) << " num output bits: " << numBits << "\n";

    // numBits is number of output bits
    // LFSR state is 2x numBits based on AOX function
    uint64_t numLFSRPatterns = 1ULL << (2*numBits);
    uint64_t numOutputPatterns = 1ULL << numBits;

    std::cout << "Num state patterns=" << numLFSRPatterns << " "
              << "num output patterns=" << numOutputPatterns << "\n";

    std::vector<uint64_t> counts(numOutputPatterns);

    // For each bit pattern.
    for (size_t pattern=0; pattern<numLFSRPatterns; pattern++) {
      uint64_t mask = numOutputPatterns - 1;
      uint64_t s0 = pattern & mask;
      uint64_t s1 = (pattern >> numBits) & mask;
      uint64_t outputPattern = aox(s0, s1, numBits) & mask;
      //uint64_t outputPattern = plus(s0, s1) & mask;
      //uint64_t outputPattern = pattern % numOutputPatterns;
      counts[outputPattern]++;
    }

    uint64_t expectedOutputs = numLFSRPatterns / numOutputPatterns;

    //// Print counts
    //for (size_t i=0; i<numOutputPatterns; i++) {
    //  std::cout<< "bin"<<i<<" count="<<counts[i]<<" E="<<expectedOutputs<<"\n";
    //}

    // Spread
    //uint64_t min = *std::min_element(counts.begin(), counts.end());
    //uint64_t max = *std::max_element(counts.begin(), counts.end());
    //std::cout << numBits << "bits, spread " << min << " " << max << " " << (max-min) << "\n";

    // Chi Squared statistic
    double chiSquared = 0.0;
    for (size_t i=0; i<numOutputPatterns; i++) {
      uint64_t countMinusExpected = counts[i] - expectedOutputs;
      chiSquared += (double)(countMinusExpected * countMinusExpected) / (double)expectedOutputs;
      //std::cout << chiSquared << "\n";
    }
    std::cout << numBits << " bits, chi2=" << chiSquared
              << ", critical=" << chi2CriticalVals[numBits] <<"\n\n";
  }

  return 0;
}
