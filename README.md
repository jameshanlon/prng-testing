# PRNG Quality Testing

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
$ ./build/pcg64_ref stdout std32 1 0 | hexdump -Cv | head
00000000  fa 7b 50 d3 2e e3 75 e1  09 31 28 0b 2a 92 bf c0  |.{P...u..1(.*...|
00000010  bb 85 87 e6 21 fa 0b 14  30 58 e3 4f cc 8b ec c5  |....!...0X.O....|
00000020  c5 69 28 a2 1c ed d8 4d  4c ef 02 c8 29 fa bf c9  |.i(....ML...)...|
00000030  8b 47 47 e1 a4 9a a0 3a  8c 37 06 2d 22 f6 fd 1d  |.GG....:.7.-"...|
00000040  4e 1d f6 4d cf da 05 5a  26 0a 84 eb b1 50 96 48  |N..M...Z&....P.H|
00000050  c8 c4 2c 91 13 1f 9e 1a  02 e6 c0 cd 98 3b ea d9  |..,..........;..|
00000060  53 8e 98 2f 9c 6c ce 67  77 58 5d 1a d4 3b 5b 2c  |S../.l.gwX]..;[,|
00000070  84 bd 12 d5 f9 dd 8c 85  80 1f cf 71 2b 66 39 3d  |...........q+f9=|
00000080  dc 20 ed 0a f9 39 f0 b0  a0 c0 52 2b 0f 43 e5 c7  |. ...9....R+.C..|
00000090  0e 40 08 e4 1b 86 45 f0  b7 c8 85 fa f5 21 62 9d  |.@....E......!b.|
```

Or in binary using `xxd`:
```bash
./build/pcg64_ref stdout std32 1 0 | xxd -b | head
00000000: 11111010 01111011 01010000 11010011 00101110 11100011  .{P...
00000006: 01110101 11100001 00001001 00110001 00101000 00001011  u..1(.
0000000c: 00101010 10010010 10111111 11000000 10111011 10000101  *.....
00000012: 10000111 11100110 00100001 11111010 00001011 00010100  ..!...
00000018: 00110000 01011000 11100011 01001111 11001100 10001011  0X.O..
0000001e: 11101100 11000101 11000101 01101001 00101000 10100010  ...i(.
00000024: 00011100 11101101 11011000 01001101 01001100 11101111  ...ML.
0000002a: 00000010 11001000 00101001 11111010 10111111 11001001  ..)...
00000030: 10001011 01000111 01000111 11100001 10100100 10011010  .GG...
00000036: 10100000 00111010 10001100 00110111 00000110 00101101  .:.7.-
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
