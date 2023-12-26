import random

NUM_SEEDS = 10
seeds_lo = []
seeds_hi = []
interval = (2**128 - 1) // NUM_SEEDS
# initial_seed = 1
initial_seed = random.randint(0, interval)
# initial_seed = 0x555555555555555555555555555555555555

# print (2**128)
# print (interval * NUM_SEEDS)
# assert interval * NUM_SEEDS == 2**128
assert (interval * NUM_SEEDS) < 2**128
# assert initial_seed < interval

for index in range(NUM_SEEDS):
    seed = int(initial_seed) + (index * interval)
    # seed = initial_seed + index
    seed_lo64 = seed & int((1 << 64) - 1)
    seed_hi64 = int(seed >> 64) & int((1 << 64) - 1)
    seeds_lo.append(seed_lo64)
    seeds_hi.append(seed_hi64)

print(f"// Period        2^128 = {2**128}")
print(f"// Interval      2^128 // {NUM_SEEDS} = {interval}")
print(f"// Initial seed  {initial_seed}")
print("uint64_t s0[NUM_GENERATORS] = {")
for x in seeds_lo:
    print(f"{x}ULL,")
print("};")
print("uint64_t s1[NUM_GENERATORS] = {")
for x in seeds_hi:
    print(f"{x}ULL,")
print("};")
