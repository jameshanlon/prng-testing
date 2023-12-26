#ifndef INTERLEAVE_SEEDING_HPP
#define INTERLEAVE_SEEDING_HPP

void set_seed_pcg64(uint64_t s0[], uint64_t s1[]) {
  // Seed randomly using PCG.
  pcg64 generator(PCG_128BIT_CONSTANT(-1, 1));
  for (size_t i = 0; i < NUM_GENERATORS; i++) {
    s0[i] = generator();
    s1[i] = generator();
  }
}

void set_seed_mt64(uint64_t s0[], uint64_t s1[]) {
  // Seed randomly using MT.
  std::mt19937 generator(0xb0801d23e66fc68e);
  for (size_t i = 0; i < NUM_GENERATORS; i++) {
    s0[i] = generator();
    s1[i] = generator();
  }
}

void set_seed_equia(uint64_t s0[], uint64_t s1[]) {
  // Set the seeds based on equidistantly-spaced intervals from 1.
  for (size_t i = 0; i < NUM_GENERATORS; i++) {
    __uint128_t const0 = 0;
    __uint128_t const1 = 1;
    __uint128_t seed = const1 + (i * (~const0 / NUM_GENERATORS));
    s0[i] = seed & 0xFFFFFFFFFFFFFFFFULL;
    s1[i] = (seed >> 64) & 0xFFFFFFFFFFFFFFFFULL;
  }
}

void set_seed_equib(uint64_t s0[], uint64_t s1[]) {
  // Set the seeds based on equidistantly-spaced intervals.
  for (size_t i = 0; i < NUM_GENERATORS; i++) {
    __uint128_t const0 = 0;
    __uint128_t const1 = 1;
    __uint128_t seed = (const1 + i) * (~const0 / (NUM_GENERATORS + 1));
    s0[i] = seed & 0xFFFFFFFFFFFFFFFFULL;
    s1[i] = (seed >> 64) & 0xFFFFFFFFFFFFFFFFULL;
  }
}

void set_seed_equic(uint64_t s0[], uint64_t s1[]) {
  // Set the seeds from half 1s/0s and an increment.
  for (size_t i = 0; i < NUM_GENERATORS; i++) {
    __uint128_t init = 0x5555555555555555ULL;
    init = init << 64;
    init = init | 0x5555555555555555ULL;
    __uint128_t seed = init + i;
    s0[i] = seed & 0xFFFFFFFFFFFFFFFFULL;
    s1[i] = (seed >> 64) & 0xFFFFFFFFFFFFFFFFULL;
  }
}

void set_seed_equid(uint64_t s0[], uint64_t s1[]) {
  // Set the seeds based on equidistantly-spaced intervals, with a RANDOM
  // offset. *Should* be no different from choosing random offsets.
  pcg64 generator(PCG_128BIT_CONSTANT(-1, 1));
  for (size_t i = 0; i < NUM_GENERATORS; i++) {
    __uint128_t const0 = 0;
    uint64_t randInitLo = generator();
    uint64_t randInitHi = generator();
    __uint128_t interval = ~const0 / NUM_GENERATORS;
    __uint128_t offset =
        (((__uint128_t)randInitHi) << 64 | (__uint128_t)randInitLo) % interval;
    __uint128_t seed = offset + (i * interval);
    s0[i] = seed & 0xFFFFFFFFFFFFFFFFULL;
    s1[i] = (seed >> 64) & 0xFFFFFFFFFFFFFFFFULL;
    // std::cout << "seed " << std::dec << i << " " << std::hex << s0[i] << " "
    // << s1[i] << "\n";
  }
}

void set_seed_equie(uint64_t s0[], uint64_t s1[]) {
  pcg64 generator(PCG_128BIT_CONSTANT(-1, 1));
  uint64_t randInitLo = generator();
  uint64_t randInitHi = generator();
  __uint128_t offset =
      (((__uint128_t)randInitHi) << 64 | (__uint128_t)randInitLo);
  __uint128_t seed = offset;
  for (size_t i = 0; i < NUM_GENERATORS; i++) {
    seed = seed + ((i * (i + 1)) / 2);
    s0[i] = seed & 0xFFFFFFFFFFFFFFFFULL;
    s1[i] = (seed >> 64) & 0xFFFFFFFFFFFFFFFFULL;
    // std::cout << "seed " << std::dec << i << " " << std::hex << s0[i] << " "
    // << s1[i] << "\n";
  }
}

#endif // INTERLEAVE_SEEDING_HPP
