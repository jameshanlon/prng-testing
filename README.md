# PRNG-testing

This repository contains facilities for comprehensively testing PRNGs using
statistical test suites. It provides a facility to run a PRNG against TestU01,
PractRand and Gjrand, with parallel runs from different seeds and permutations
of the output bits, and a script for summarising results across all the runs.

## Getting started

To build the PRNG scripts, you must first install the dependencies, which
includes both the PRNG libraries as well as the Test libs TestU01, PractRand
and gjrand.

To install the dependencies, run:

```bash
$ bash install_deps.sh
```

Some manual intervention may be required.

Then, build the PRNG executables:

```bash
$ make -j8
```

Each generator must specify four arguments:

```
<command> <output> <seed64lo> <seed64hi>
```

The command must be one of `stdout`, `smallcrush`, `crush`, `bigcrush`.

The output must be one of `std32`, `rev32`, `std64`, `rev64`, `std32lo`,
`rev32lo`, `std32hi`, `rev32hi`.

Example smallcrush test:

```bash
$ ./pcg64_ref smallcrush std32 1 0
```

Example hexdump of stdout:

```bash
$ ./pcg64_ref stdout std32 1 0 | hexdump -Cv | head
```

Or in binary:

```bash
./bin/pcg64_ref stdout std32 1 0 | xxd -b | head
```

Example run of a set of seeds:

```bash
$ python3 run_tests.py TEST_ZEROS_SMALL_10 zero_generator smallcrush std32 --numseeds 10
...
$ python3 summarise_results.py TEST_ZEROS_SMALL_10/ testu01 --verbose
10       results
10       total seed failure(s)
150      total test failure(s)
0        total tests run
```

A note on Philox: to make our implementation (`philox4x32`) equal the reference
implementation (`philox4x32_ref`), the keys k0 and k1 must be zeroed after the
ten philox rounds have been completed, i.e after the "for" loop in the function
`next_philox2x64_10(void)` and `next_philox4x32_10(void)`. The `ctr.incr()
function must also be replaced with a function that increments ctr[2].
