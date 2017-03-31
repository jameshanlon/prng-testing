# Testing pseudorandom number generators

This repository contains a simple example of using TestU01 to assess the
quality of a pseudorandom number generator, as part of an [article I wrote on
testing
PRNGs](http://www.jwhanlon.com/notes-on-testing-random-number-generators.html).
It replicates the testing of the `xososhiro128+` generator as detailed on the
[PRNG shootout page](http://xoroshiro.di.unimi.it), but can be easily modified
to test other generators.

Assuming you have the tools `wget`, `unzip`, `make`, `gcc` and `python`, you
can run the following commands to run the example:
```
$ ./install-testu01.sh && make
...
$ ./run_tests.py crush-std results && ./summarise_results.py results
...
```
