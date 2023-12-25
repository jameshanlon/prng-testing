#include <iostream>
#include <vector>

extern "C" {
#include "smultin.h"
#include "gdef.h"
#include "unif01.h"
#include "sres.h"
#include "sknuth.h"
}

constexpr size_t MIN_NUM_BITS = 2;//2;
constexpr size_t MAX_NUM_BITS = 2; // Need to modify rand32 to go higher

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
  //uint64_t outputPattern = aox(s0, s1, g_numBits) & mask;
  uint64_t outputPattern = plus(s0, s1) & mask;
  std::cout << std::hex << outputPattern << "\n";
  g_pattern++;
  return outputPattern;
}

int main(int argc, const char *argv[]) {

  for (size_t numBits=MIN_NUM_BITS; numBits<=MAX_NUM_BITS; numBits++) {
    std::cout << "Num state bits " << (numBits*2) << " num output bits: " << numBits << "\n";

    // numBits is number of output bits
    // LFSR state is 2x numBits based on AOX function
    //uint64_t numLFSRPatterns = 1ULL << (2*numBits);
    uint64_t numOutputPatterns = 1ULL << numBits;

    // Setup generator.
    g_pattern = 0;
    g_numPatterns = numOutputPatterns;
    g_numBits = numBits;

    // Setup sknuth_Serial test.
    //sres_Chi2 *res = nullptr;
    //char *name = "AOX";
    //unif01_Gen *gen = unif01_CreateExternGenBits(name, rand32);
    //long N = 1;
    //long n = numOutputPatterns;
    //int r = 0;  // Discard r bits from each output?
    //long d = numOutputPatterns;
    //int t = numBits;
    //sknuth_Serial(gen, res, N, n, r, d, t);
    //unif01_DeleteExternGenBits(gen);

    // Setup smultin_MultinomialBits
    smultin_Param *par = nullptr;
    smultin_Res *res = nullptr;
    int NbDelta = 1;
    double ValDelta[] = { -1 };
    long N = 1;
    long n = (numOutputPatterns) * numBits;
    long L = numBits; // Take L bits from each s to determine a cell number (2^L cells).
    int r = numBits;  // Discard r bits from each output.
    int s = numBits; // Take s bits from each output.
    par = smultin_CreateParam(NbDelta, ValDelta, smultin_GenerCellSerial, -1);
    char *name = "AOX";
    unif01_Gen *gen = unif01_CreateExternGenBits(name, rand32);
    smultin_MultinomialBits(gen, par, res, N, n, r, s, L, TRUE);
    unif01_DeleteExternGenBits(gen);
  }
  return 0;
}
