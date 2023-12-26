# PRNG-testing

This repository contains facilities for comprehensively testing PRNGs using
statistical test suites. It provides a facility to run a PRNG against TestU01,
PractRand and Gjrand, with parallel runs from different seeds and permutations
of the output bits, and a script for summarising results across all the runs.

The code in this repository was originally used to produce the results for the
paper [*A Fast Hardware Pseudorandom Number Generator Based on
xoroshiro128*](https://arxiv.org/abs/2203.04058), James Hanlon and Stephen
Felix in IEEE Transactions on Computers, 2022.

## Getting started

To build the PRNG scripts, you must first install the dependencies, which
includes both the PRNG libraries as well as the Test libs TestU01, PractRand
and gjrand. This has not been tested on may platforms so some manual
intervention may be required.

To install the dependencies, run:

```bash
$ bash install_deps.sh
...
```

Setup the Python environment:

```bash
$ python -m venv env
$ source env/bin/activate
$ pip install -r requirements.txt
...
```

Build the PRNG executables:

```bash
$ mkdir build
$ cd build
$ cmake ..
$ make -j32
...
```

Run the unit tests:
```bash
$ ctest
...
```

## Generators

The build produces a set of generators as individual executable binaries. Each
one provides the same command line interface, which requires four positional
arguments, eg:

```
$ ./zero_generator -h
./zero_generator command output seed64lo seed64hi

Positional parameters:
  command  [stdout, smallcrush, crush, bigcrush, alphabit, matrixrank, linearcomp, test, time, analyse]
  output   [std32, rev32, std64, rev64, std32lo, rev32lo, std32hi, rev32hi]
  seed64hi 64 lowest bits of the 128-bit seed
  seed64lo 64 highest bits of the 128-bit seed

Optional parameters:
  --output-shift N  Number of bits to shift the generator output by
```

The generators can produce output to `stdout`, which can be inspected for
example using `hexdump`:
```bash
$ ./pcg64_ref stdout std32 1 0 | hexdump -Cv | head
```

Or in binary using `xxd`:
```bash
./bin/pcg64_ref stdout std32 1 0 | xxd -b | head
```

Other commands run test sets against the generator. For example, to run the
`smallcrush` test set for a single seed:
```bash
$ ./pcg64_ref smallcrush std32 1 0
...
```

The `run_test_set.py` script can be used to run a test set over a set of seeds:
```bash
$ python3 run_test_set.py TEST_ZEROS_SMALL_10 zero_generator smallcrush std32 --numseeds 10
...
```

And the `summarise_results.py` can be used to parse the result output and
produce a succinct summary:
```
$ python3 summarise_results.py TEST_ZEROS_SMALL_10/ testu01 --verbose
10       results
10       total seed failure(s)
150      total test failure(s)
0        total tests run
```

The `run_wrapper.py` script is a wrapper around `run_test_set.py` to run the
complete set of tests (ie seeds and output types) against a specific test set.

### A note on Philox

To make the included implementation (`philox4x32`) equal the reference
implementation (`philox4x32_ref`), the keys k0 and k1 must be zeroed after the
ten philox rounds have been completed, i.e after the "for" loop in the function
`next_philox2x64_10(void)` and `next_philox4x32_10(void)`. The `ctr.incr()`
function must also be replaced with a function that increments `ctr[2]`. This
is captured by one of the tests.

## Licensing

Unless otherwise noted, all code in this repository is licensed under the
Apache 2.0 license. See [LICENSE](LICENSE) for details.
