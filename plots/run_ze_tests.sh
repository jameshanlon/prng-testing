set -x
# 1k steps
./build/ze_mersenne_twister 1000 1 1 > ze-1k-mt32.dat
./build/ze_pcg64_ref 1000 1 1 > ze-1k-pcg64.dat
./build/ze_philox4x32_ref 1000 1 1 > ze-1k-philox4x32.dat
./build/ze_xoroshiro128plus_55_14_36 1000 1 1 > ze-1k-plus.dat
./build/ze_xoroshiro128aox_55_14_36 1000 1 1 > ze-1k-aox.dat
# 1m steps, 1k samples
./build/ze_mersenne_twister 1000000 1000 1 > ze-1m-mt32.dat
./build/ze_pcg64_ref 1000000 1000 1 > ze-1m-pcg64.dat
./build/ze_philox4x32_ref 1000000 1000 1 > ze-1m-philox4x32.dat
./build/ze_xoroshiro128plus_55_14_36 1000000 1000 1 > ze-1m-plus.dat
./build/ze_xoroshiro128aox_55_14_36 1000000 1000 1 > ze-1m-aox.dat
